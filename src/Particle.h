#pragma once

#include "ofMain.h"
#include "ofGraphics.h"
#include "ofPoint.h"
#include "FlowField.h"

/** TODO
 * 3D boundingBox
 * 3 rules for swarm behaviour
 * state dependent settings
 * 		settings setter function
 * 		change settings when change state!
 * performance
 * 		limit()
 * 		normalize()
 * 		magnitude()
 * 		distance()
 * steer vs addOriginForce?!
 */
inline float fastInvSqrt(float x) {
	float xhalf = 0.5f * x;
	int i = *(int*) &x; // store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
	x = *(float*) &i; // convert new bits into float
	x = x * (1.5f - xhalf * x * x); // One round of Newton's method
	return x;
}

inline float fastInvLength(ofVec3f & vec) {
	return fastInvSqrt(vec.lengthSquared());
}

inline float fastLength(ofVec3f & vec) {
	return 1 / fastInvLength(vec);
}

inline float fastDist(ofVec3f & target, ofVec3f & loc) {
	ofVec3f vec = target - loc;
	return fastLength(vec);
}

inline void fastNormalize(ofVec3f & vec, float preCalcLength = -1) {
	if (preCalcLength == -1) { //not precalulated
		vec *= fastInvLength(vec);
	} else {
		vec /= preCalcLength;
	}
}

inline void fastLimit(ofVec3f & vec, float limit, float preCalcLength = -1) {
	float length;
	if (preCalcLength == -1) { //not precalculated
		length = fastLength(vec);
	} else {
		length = preCalcLength;
	}

	if (length > limit) {
		vec /= length;
		vec *= limit;
	}
}

inline void printFunctionTestResults() {
	cout << "function tests:" << endl;
	float square = 16;
	cout << "root of " << square << " = " << 1 / fastInvSqrt(square) << endl;
	ofVec3f vec(4, 4, 4);
	cout << "compare length: " << vec.length() << " with " << fastLength(vec)
			<< endl;
	ofVec3f vec2(2, 2, 0);
	cout << "compare distane: " << vec.distance(vec2) << " with "
			<< fastDist(vec, vec2) << endl;
	fastNormalize(vec);
	cout << "test normalize - " << "length after normalize: " << fastLength(vec)
			<< endl;
	fastLimit(vec2, 0.2);
	cout << "test limit - " << "length after limit to 0.2: " << fastLength(vec2)
			<< endl;
}

typedef enum {
	PARTICLE_FREE,
//	PARTICLE_FLOCKING,
//	PARTICLE_EXPLODE,
//	PARTICLE_TARGET,
//	PARTICLE_REST
	PARTICLE_ORIGIN,
	PARTICLE_ZLAYER

} particleState;

class SwarmParticle: public ofPoint {

public:
	ofColor c;
	ofVec3f vel;
	ofVec3f acc;
	ofPoint origin;

	float radius;
	float alpha;
	float maxSpeed; //TODO wrong naming - it's more something like forceFraction - don't know...
	float speedLimit;
	float maxForce;

	float separatorForce;
	float alignForce;
	float cohesionForce;
	float desiredSeparation;
	float alignRange;
	float cohesionRange;

	float zForce;

	particleState state;

	SwarmParticle(float _x = 0, float _y = 0, float _xv = 0, float _yv = 0);

	virtual ~SwarmParticle() {
	}

	virtual bool isActive() {
		return bActive;
	}

	virtual void setActive(bool free,bool moveZ = false);

	virtual bool ignoresForces() {
		return bIgnoreForce;
	}

	virtual bool isUsed() {
		return bUsed;
	}

	virtual void setUsed(bool val) {
		bUsed = val;
	}

	//gravity force to additional defined origin
	//TODO faster(invSQRT)
	//TODO smaller force, effect = (length/size)*force with size ≃ windowsize
	//		addOriginForce(force,size)
	//REVISIT same as "arrival"?
	virtual void addOriginForce(float scale);

	// Implementing Reynolds' flow field following algorithm
	// http://www.red3d.com/cwr/steer/FlowFollow.html
	virtual void follow(FlowField f);

	//We accumulate a new acceleration each time based on three rules
	void flock(vector<SwarmParticle*> & boids);
	void oldflock(vector<SwarmParticle*> & boids);

	// Separation
	// Method checks for nearby boids and steers away
	ofVec3f separate(vector<SwarmParticle*> & boids);

	// Alignment
	// For every nearby boid in the system, calculate the average velocity
	ofVec3f align(vector<SwarmParticle*> & boids);

	// Cohesion
	// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
	ofVec3f cohesion(vector<SwarmParticle *> & boids);

	// A method that calculates a steering vector towards a target
	// Takes a second argument, if true, it slows down as it approaches the target
	ofVec3f steer(ofVec3f target, bool slowdown);

	//check for origins which are not inside the world
	void checkOrigin(float minX, float minY, float maxX, float maxY, float minZ,
			float maxZ) {

		if (origin.x < minX || origin.x > maxX || origin.y < minY
				|| origin.y > maxY || origin.z < minZ || origin.z > maxZ) {
			state = PARTICLE_ZLAYER;
			setUsed(false);
		}
	}

	// Wraparound
	void borders(float minX, float minY, float maxX, float maxY, float minZ,
			float maxZ) {
		if (x < minX || x > maxX)
			vel.x *= -1;
		if (y < minY || y > maxY)
			vel.y *= -1;
		if (z < minZ || z > maxZ)
			vel.z *= -1;

//		checkOrigin(minX, minY, maxX, maxY, minZ, maxZ);
		//TODO check is by the system checkBorders?!

		//durchfluss
//	    if (x < minX) x = maxX;
//	    if (y < minY) y = maxY;
//	    if (x > maxX) x = minX;
//	    if (y > maxY) y = minY;
////	    if (z < minZ) vel.z *= -1;
////	    if (z > maxZ) vel.z *= -1;
//		if (z < minZ) z = maxZ;
//		if (z > maxZ) z = minZ;
	}

	virtual void updatePosition(float timeStep);

	void resetForce();

	void addDampingForce(float damping = .01);

	virtual void draw(float grey = 255);

	virtual void drawVertex();

protected:

	ofVec3f lookup;
	ofVec3f cohesionSum;
	ofVec3f cohesionSteer;

	bool bActive;
	bool bIgnoreForce;
	bool bKillSoft;
	bool bUsed;

};
