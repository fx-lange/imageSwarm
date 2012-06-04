#include "Particle.h"

SwarmParticle::SwarmParticle(float _x , float _y , float _xv, float _yv ) :
		ofPoint(_x,_y){
	bFree = true;
	bIgnoreForce = true;
	alpha = 0;
	radius = 2;
	origin.set(_x,_y);
	vel.set(_xv,_yv);
	state = PARTICLE_FREE;
	bUsed = false;
}

void SwarmParticle::setFree(bool free){
	bFree = free;
	if(bFree){
		alpha = 0;
		z=0;
		bIgnoreForce = true;
	}else{
		bIgnoreForce = false;
		alpha = 255;
		z=0;
	}
	bKillSoft = false;
}

//gravity force to additional defined origin
//TODO faster(invSQRT)
//TODO smaller force, effect = (length/size)*force with size ≃ windowsize
//		addOriginForce(force,size)
void SwarmParticle::addOriginForce(float scale){
	if(state != PARTICLE_ORIGIN){
		return;
	}

	float xd = x - origin.x;
	float yd = y - origin.y;
	float zd = z - 0;
	float length = xd * xd + yd * yd + zd * zd;
	if(length > 0 ){
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

		length = sqrtf(length);//TODO performance
		xd /= length;
		yd /= length;
		zd /= length;
		float effect = length * scale;
		xd *= effect;
		yd *= effect;
		zd *= effect;
		acc.x += xd;
		acc.y += yd;
		acc.z += zd;
	}
}

void SwarmParticle::updatePosition(float timeStep) {
	if(bFree)
		return;
	// f = ma, m = 1, f = a, v = int(a)
	vel += acc;
	x += vel.x * timeStep;
	y += vel.y * timeStep;
	z += vel.z * timeStep;

}

void SwarmParticle::resetForce() {
	acc.set(0,0,0);
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
	desired.scale(maxSpeed);
	// Steering is desired minus velocity
//	    PVector steer = PVector.sub(desired, vel);
	desired -= vel;
//	    desired.limit(maxforce*flowfieldForce);  //TODO Limit to maximum steering force
	desired *= 0.1; //TODO just temporary
	acc += desired;
}

void SwarmParticle::draw(float grey ) {
	if(bFree){
		return;
	}
	ofSetColor(grey,grey,grey,alpha);//TODO include z
	ofSphere(x,y,z,radius);
	ofSetColor(255,0,0);
	ofSphere(lookup,2);//TODO debug
}

void SwarmParticle::drawVertex(){
	if(bFree){
		return;
	}
	ofSetColor(255,255,255,alpha);
	glVertex3f(x,y,z);
}
