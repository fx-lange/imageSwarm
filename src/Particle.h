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

typedef enum {
	PARTICLE_FREE,
//	PARTICLE_FLOCKING,
//	PARTICLE_EXPLODE,
//	PARTICLE_TARGET,
//	PARTICLE_REST
	PARTICLE_ORIGIN
} particleState;

class SwarmParticle: public ofPoint {

public:
	ofVec3f vel;
	ofVec3f acc;
	ofPoint origin;

	float radius;
	float alpha;
	float maxSpeed;

	float separatorForce;
	float alignForce;
	float cohesionForce;

	particleState state;

	SwarmParticle(float _x = 0, float _y = 0, float _xv = 0, float _yv = 0);

	virtual ~SwarmParticle() {
	}

	virtual bool isFree() {
		return bFree;
	}

	virtual void setFree(bool free);

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
	void flock(vector<SwarmParticle*> boids);

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
	ofVec3f steer(ofVec3f target, bool slowdown) ;

	// Wraparound
	  void borders(float minX,float minY,float maxX, float maxY, float minZ, float maxZ) {
	    if (x < minX) x = maxX;
	    if (y < minY) y = maxY;
	    if (x > maxX) x = minX;
	    if (y > maxY) y = minY;
	    if (z < minZ) vel.z *= -1;
	    if (z > maxZ) vel.z *= -1;
	  }

	virtual void updatePosition(float timeStep);

	void resetForce();

	void addDampingForce(float damping = .01);

	virtual void draw(float grey = 255);

	virtual void drawVertex();

protected:

	ofVec3f lookup;

	bool bFree;
	bool bIgnoreForce;
	bool bKillSoft;
	bool bUsed;

};

inline float InvSqrt(float x) {
	float xhalf = 0.5f * x;
	int i = *(int*) &x; // store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
	x = *(float*) &i; // convert new bits into float
	x = x * (1.5f - xhalf * x * x); // One round of Newton's method
	return x;
}
