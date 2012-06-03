#include "Particle.h"

SwarmParticle::SwarmParticle(float _x , float _y , float _xv, float _yv ) :
		ofPoint(_x,_y){
	bFree = true;
	bIgnoreForce = true;
	alpha = 0;
	radius = 2;
	origin.set(_x,_y);
	vel.set(_xv,_yv);
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

		length = sqrtf(length);
		xd /= length;
		yd /= length;
		zd /= length;
		float effect = length * scale;
		xd *= effect;
		yd *= effect;
		zd *= effect;
		steer.x += xd;
		steer.y += yd;
		steer.z += zd;
	}
}

void SwarmParticle::updatePosition(float timeStep) {
	if(bFree)
		return;
	// f = ma, m = 1, f = a, v = int(a)
	vel += steer;
	x += vel.x * timeStep;
	y += vel.y * timeStep;
	z += vel.z * timeStep;

}

void SwarmParticle::resetForce() {
	steer.set(0,0,0);
}

void SwarmParticle::addDampingForce(float damping) {
	steer = steer - vel * damping;
}

void SwarmParticle::draw(float grey ) {
	if(bFree){
		return;
	}
	ofSetColor(grey,grey,grey,alpha);//TODO include z
	ofCircle(x,y,radius);
}

void SwarmParticle::drawVertex(){
	if(bFree){
		return;
	}
	ofSetColor(255,255,255,alpha);
	glVertex3f(x,y,z);
}
