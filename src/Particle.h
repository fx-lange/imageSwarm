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
	void flock(vector<SwarmParticle*> boids) {
		ofVec3f sep = separate(boids); // Separation
		ofVec3f ali = align(boids); // Alignment
		ofVec3f coh = cohesion(boids); // Cohesion

		// Arbitrarily weight these forces
		float scatter = ofRandom(0.2);
		sep *= (separatorForce + scatter);
		ali *= (alignForce + scatter);
		coh *= (cohesionForce + scatter);

		// Add the force vectors to acceleration
		acc += sep;
	}

	// Separation
	// Method checks for nearby boids and steers away
	ofVec3f separate(vector<SwarmParticle*> & boids) {
		float desiredseparation = 20.0; //TODO GUI
		ofVec3f steer(0, 0, 0);
		int count = 0;
		// For every boid in the system, check if it's too close
		for (int i = 0; i < boids.size(); i++) {
			ofVec3f * other = boids[i];
			float d = this->distance(*other);
			// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
			if ((d > 0) && (d < desiredseparation)) {
				// Calculate vector pointing away from neighbor
//				ofVec3f diff = PVector.sub(loc, other.loc);
				ofVec3f diff = *this - *other;
				diff.normalize();
				diff /= d; // Weight by distance
				steer += diff;
				count++; // Keep track of how many
			}
		}
		// Average -- divide by how many
		if (count > 0) {
			steer /= (float) count;
		}

		// As long as the vector is greater than 0
		if (steer.length() > 0) {
			// Implement Reynolds: Steering = Desired - Velocity
			steer.normalize();
			steer *= maxSpeed;
			steer -= vel;
//			steer.limit(maxforce); //TODO
		}
		return steer;
	}

	// Alignment
	// For every nearby boid in the system, calculate the average velocity
	ofVec3f align(vector<SwarmParticle*> & boids) {
		float neighbordist = 25.0; //TODO gui
		ofVec3f steer(0, 0, 0);
		int count = 0;
		for (int i = 0; i < boids.size(); i++) {
			SwarmParticle * other = boids[i];
			float d = this->distance(*other);
			if ((d > 0) && (d < neighbordist)) {
				steer += other->vel;
				count++;
			}
		}
		if (count > 0) {
			steer /= (float) count;
		}

		// As long as the vector is greater than 0
		if (steer.length() > 0) {
			// Implement Reynolds: Steering = Desired - Velocity
			steer.normalize();
			steer *= maxSpeed;
			steer -= vel;
//	      steer.limit(maxforce); //TODO
		}
		return steer;
	}

	// Cohesion
	// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
	ofVec3f cohesion(vector<SwarmParticle *> & boids) {
		float neighbordist = 25.0;
		ofVec3f sum(0, 0, 0); // Start with empty vector to accumulate all locations
		int count = 0;
		for (int i = 0; i < boids.size(); i++) {
			SwarmParticle * other = boids[i];
			float d = this->distance(*other);
			if ((d > 0) && (d < neighbordist)) {
				sum += *other; // Add location
				count++;
			}
		}
		if (count > 0) {
			sum /= (float) count;
			return steer(sum, false); // Steer towards the location
		}
		return sum;
	}

	// A method that calculates a steering vector towards a target
	// Takes a second argument, if true, it slows down as it approaches the target
	ofVec3f steer(ofVec3f target, bool slowdown) {
		ofVec3f steer; // The steering vector
		ofVec3f desired = target - *this; // A vector pointing from the location to the target
		float d = desired.length(); // Distance from the target is the magnitude of the vector
		// If the distance is greater than 0, calc steering (otherwise return zero vector)
		if (d > 0) {
			// Normalize desired
			desired.normalize();
			// Two options for desired vector magnitude (1 -- based on distance, 2 -- maxspeed)
			if ((slowdown) && (d < 100.0))
				desired *= (maxSpeed * (d / 100.0)); // This damping is somewhat arbitrary
			else
				desired *= maxSpeed;
			// Steering = Desired minus PVectorVelocity
			steer = desired - vel;
//			steer.limit(maxforce); //TODO Limit to maximum steering force
		} else {
			steer.set(0, 0, 0);
		}
		return steer;
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
