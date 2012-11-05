#include "Particle.h"

SwarmParticle::SwarmParticle(float _x, float _y, float _xv, float _yv) :
		ofPoint(_x, _y) {
	bActive = false;
	bIgnoreForce = true;
	alpha = 0;
	radius = 2;
	origin.set(_x, _y);
	vel.set(_xv, _yv);
	state = PARTICLE_ZLAYER;
	bUsed = false;
}

void SwarmParticle::setActive(bool active, bool moveZ) {
	bActive = active;
	if (bActive) {
		bIgnoreForce = false;
		alpha = 255;
//		z = 0;
	} else {
		alpha = 0;
		if(moveZ){
			z = ofRandom(50,150);
			x += ofRandom(-200,200);
			y += ofRandom(-200,200);
		}
		bIgnoreForce = true;
	}
	bKillSoft = false;
}

//gravity force to additional defined origin
//TODO faster(invSQRT)
//TODO smaller force, effect = (length/size)*force with size ≃ windowsize
//		addOriginForce(force,size)
void SwarmParticle::addOriginForce(float scale) {
	if (state != PARTICLE_ORIGIN && state != PARTICLE_ZLAYER ) {
		return;
	}

	float xd = x - origin.x;
	float yd = y - origin.y;
	float zd = z - origin.z;
	float length = 0;
	if(state == PARTICLE_ORIGIN){
		length = xd * xd + yd * yd + zd * zd;
	}else if(state == PARTICLE_ZLAYER){
		length = zd * zd;
	}
	if (length > 0) {
//			float xhalf = 0.5f * length;
//			int lengthi = *(int*) &length;
//			lengthi = 0x5f3759df - (lengthi >> 1);
//			length = *(float*) &lengthi;
//			length *= 1.5f - xhalf * length * length;
//			xd *= length;
//			yd *= length;
//			length *= radius;
//			length = 1 / length;
////			length = (1 - length);
//			length *= force;
//			xd *= length;
//			yd *= length;

		length = sqrtf(length); //TODO performance - use fastNormalize - compare with ParticleSystem::addForce
		xd /= length;
		yd /= length;
		zd /= length;

		if(state == PARTICLE_ORIGIN){
			float effect = length * scale;
			xd *= effect;
			yd *= effect;
			zd *= effect;
			acc.x += xd;
			acc.y += yd;
			acc.z += zd;
		}else if(state == PARTICLE_ZLAYER){
			float effect = length * settings->zForce;
			zd *= effect;
			acc.z += zd;
		}
	}
}

void SwarmParticle::updatePosition(float timeStep) {
	if (!bActive)
		return;
	// f = ma, m = 1, f = a, v = int(a)
	vel += acc;
	x += vel.x * timeStep;
	y += vel.y * timeStep;
	z += vel.z * timeStep;

}

void SwarmParticle::resetForce() {
	acc.set(0, 0, 0);
}

void SwarmParticle::addDampingForce(float damping) {
	acc = acc - vel * damping;
}

// Implementing Reynolds' flow field following algorithm
// http://www.red3d.com/cwr/steer/FlowFollow.html
void SwarmParticle::follow(FlowField f) {

	// Look ahead
	lookup = vel;
	lookup.normalize();
	lookup.scale(32); // Arbitrarily look 32 pixels ahead
	lookup += ofVec3f(x, y, z); //TODO += *this?

	// What is the vector at that spot in the flow field?
	ofVec3f desired = f.lookup(lookup);
	// Scale it up by maxspeed
	desired.scale(settings->maxSpeed);
	// Steering is desired minus velocity
//	    PVector steer = PVector.sub(desired, vel);
	desired -= vel;
//	    desired.limit(maxforce*flowfieldForce);  //TODO Limit to maximum steering force
	desired *= 0.1; //TODO just temporary
	acc += desired;
}

//We accumulate a new acceleration each time based on three rules
void SwarmParticle::oldflock(vector<SwarmParticle*> & boids) {
	if(!isActive() || state == PARTICLE_ORIGIN)
		return;
	ofVec3f sep = separate(boids); // Separation
	ofVec3f ali = align(boids); // Alignment
	cohesionSteer = cohesion(boids); // Cohesion

	// Arbitrarily weight these forces
	float scatter = ofRandom(0.0);
	sep *= (settings->separatorForce + scatter);
	ali *= (settings->alignForce + scatter);
	cohesionSteer *= (settings->cohesionForce + scatter);

	// Add the force vectors to acceleration
	acc += sep;
	acc += ali;
	acc += cohesionSteer;
}

void SwarmParticle::flock(vector<SwarmParticle*> & boids){

	ofVec3f align(0, 0, 0);
	int countAlign = 0;

	//separation
	ofVec3f sep(0, 0, 0);
	int countSeparate = 0;

	//cohesion
	int countCohesion = 0;
	cohesionSum.set(0, 0, 0); //Start with empty vector to accumulate all locations

	for (unsigned int i = 0; i < boids.size(); i++) {
		if(!boids[i]->isActive())
				continue;

		SwarmParticle * other = boids[i];
		float d = fastDist(*this,*other);

		if(d>0){
			//cohesion
			if (d < settings->cohesionRange) {
				cohesionSum += *other; // Add location
				countCohesion++;
			}

			//separate
			if (d < settings->desiredSeparation) {
				// Calculate vector pointing away from neighbor
				ofVec3f diff = *this - *other;
				fastNormalize(diff);
				diff /= d; // Weight by distance
				sep += diff;
				countSeparate++; // Keep track of how many
			}

			//align
			if (d < settings->alignRange) {
				align += other->vel;
				countAlign++;
			}
		}
	}

	//separate
		// Average -- divide by how many
		if (countSeparate > 0) {
			sep /= (float) countSeparate;
		}
		// As long as the vector is greater than 0
		float sepLength = fastLength(sep);
		if (sepLength > 0) {
			// Implement Reynolds: Steering = Desired - Velocity
			fastNormalize(sep,sepLength);
			sep *= settings->maxSpeed;
			sep -= vel;
			fastLimit(sep,settings->maxForce);
		}

	//cohesion
		if (countCohesion > 0) {
			cohesionSum /= (float) countCohesion;
			cohesionSteer = steer(cohesionSum, false); // Steer towards the location
		}

	//align
		if (countAlign > 0) {
			align /= (float) countAlign;
		}
		// As long as the vector is greater than 0
		float alignLength = fastLength(align);
		if (alignLength > 0) {
			// Implement Reynolds: Steering = Desired - Velocity
			fastNormalize(align,alignLength);
			align *= settings->maxSpeed;
			align -= vel;
			fastLimit(align,settings->maxForce);
		}

	float scatter = ofRandom(0.0);
	sep *= (settings->separatorForce + scatter);
	align *= (settings->alignForce + scatter);
	cohesionSteer *= (settings->cohesionForce + scatter);

	// Add the force vectors to acceleration
	acc += sep;
	acc += align;
	acc += cohesionSteer;
}

// Separation
// Method checks for nearby boids and steers away
ofVec3f SwarmParticle::separate(vector<SwarmParticle*> & boids) {
	float desiredseparation = 20.0; //TODO GUI
	ofVec3f steer(0, 0, 0);
	int count = 0;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size(); i++) {
		if(!boids[i]->isActive())
			continue;
		ofVec3f * other = boids[i];
		float d = fastDist(*this,*other);
		// If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
		if ((d > 0) && (d < desiredseparation)) {
			// Calculate vector pointing away from neighbor
//				ofVec3f diff = PVector.sub(loc, other.loc);
			ofVec3f diff = *this - *other;
			fastNormalize(diff);
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
	float steerLength = fastLength(steer);
	if (steerLength > 0) {
		// Implement Reynolds: Steering = Desired - Velocity
		fastNormalize(steer,steerLength);
		steer *= settings->maxSpeed;
		steer -= vel;
		fastLimit(steer,settings->maxForce);
	}
	return steer;
}

// Alignment
// For every nearby boid in the system, calculate the average velocity
ofVec3f SwarmParticle::align(vector<SwarmParticle*> & boids) {
	float neighbordist = 25.0; //TODO gui
	ofVec3f steer(0, 0, 0);
	int count = 0;
	for (int i = 0; i < boids.size(); i++) {
		if(!boids[i]->isActive())
				continue;
		SwarmParticle * other = boids[i];
		float d =fastDist(*this,*other);
		if ((d > 0) && (d < neighbordist)) {
			steer += other->vel;
			count++;
		}
	}
	if (count > 0) {
		steer /= (float) count;
	}

	// As long as the vector is greater than 0
	float steerLength = fastLength(steer);
	if (steerLength > 0) {
		// Implement Reynolds: Steering = Desired - Velocity
		fastNormalize(steer,steerLength);
		steer *= settings->maxSpeed;
		steer -= vel;
		fastLimit(steer,settings->maxForce);
	}
	return steer;
}

// Cohesion
// For the average location (i.e. center) of all nearby boids, calculate steering vector towards that location
ofVec3f SwarmParticle::cohesion(vector<SwarmParticle *> & boids) {
	float neighbordist = 50.0; //TODO GUI
	cohesionSum.set(0, 0, 0); // Start with empty vector to accumulate all locations
	int count = 0;
	for (unsigned int i = 0; i < boids.size(); i++) {
		if(!boids[i]->isActive())
				continue;
		SwarmParticle * other = boids[i];
		float d = fastDist(*this,*other);
		if ((d > 0) && (d < neighbordist)) {
			cohesionSum += *other; // Add location
			count++;
		}
	}
	if (count > 0) {
		cohesionSum /= (float) count;
		return steer(cohesionSum, false); // Steer towards the location
	}
	return cohesionSum;
}

// A method that calculates a steering vector towards a target
// Takes a second argument, if true, it slows down as it approaches the target
ofVec3f SwarmParticle::steer(ofVec3f target, bool slowdown) {
	ofVec3f steer; // The steering vector
	ofVec3f desired = target - *this; // A vector pointing from the location to the target
	float d = fastLength(desired); // Distance from the target is the magnitude of the vector
	// If the distance is greater than 0, calc steering (otherwise return zero vector)
	if (d > 0) {
		// Normalize desired
		fastNormalize(desired,d);
		// Two options for desired vector magnitude (1 -- based on distance, 2 -- maxspeed)
		if ((slowdown) && (d < 100.0))
			desired *= (settings->maxSpeed * (d / 100.0)); // This damping is somewhat arbitrary
		else
			desired *= settings->maxSpeed;
		// Steering = Desired minus PVectorVelocity
		steer = desired - vel;
		fastLimit(steer,settings->maxForce); //TODO Limit to maximum steering force
	} else {
		steer.set(0, 0, 0);
	}
	return steer;
}

void SwarmParticle::draw(float grey) {
	if (!bActive) {
		return;
	}
	ofSetColor(grey, grey, grey, alpha); //TODO include z
	ofCircle(x, y, z, radius);
//	ofSetColor(255, 0, 0);
////	ofCircle(lookup, 2); //TODO debug
//	ofLine(*this,cohesionSum);
//	ofCircle(cohesionSum,3);
//
//	ofSetColor(0,0,255);
//	ofLine(*this,*this+cohesionSteer);
}

void SwarmParticle::drawVertex() {
	if (!bActive) {
		return;
	}
//	ofSetColor(255);
	ofSetColor(c,alpha);
//	ofSetColor(255,c.g < 255 ? 0 : 255, 255, alpha);
	glVertex3f(x, y, z);
	ofSetColor(255);
}
