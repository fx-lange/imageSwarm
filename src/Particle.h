#pragma once

#include "ofMain.h"
#include "ofGraphics.h"
#include "ofPoint.h"

class SwarmParticle : public ofPoint {
public:
	ofVec3f vel;
	ofVec3f force;

//	float xv, yv;
//	float xf, yf;

	float radius;
	float alpha;
	bool bFree;
	bool bNoForce;
	bool bKillSoft;

	ofPoint origin; //


	SwarmParticle(float _x = 0, float _y = 0, float _xv = 0, float _yv = 0) :
			ofPoint(_x,_y){
		bFree = true;
		bNoForce = true;
		alpha = 0;
		radius = 2;
		origin.set(_x,_y);
		vel.set(_xv,_yv);
	}

	virtual ~SwarmParticle(){

	}


	virtual void setFree(bool free){
		bFree = free;
		if(bFree){
			alpha = 0;
			z=0;
			bNoForce = true;
		}else{
			bNoForce = false;
			alpha = 255;
			z=0;
//			cout << "particle reserved" << endl;
		}
		bKillSoft = false;
	}

	//gravity force to additional defined origin
	//TODO faster(invSQRT)
	//TODO smaller force, effect = (length/size)*force with size ≃ windowsize
	//		addOriginForce(force,size)
	virtual void addOriginForce(float scale){
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
			force.x += xd;
			force.y += yd;
			force.z += zd;
		}
	}

	virtual void updatePosition(float timeStep) {
		if(bFree)
			return;
		// f = ma, m = 1, f = a, v = int(a)
		vel += force;
		x += vel.x * timeStep;
		y += vel.y * timeStep;
		z += vel.z * timeStep;

	}

	void resetForce() {
		force.set(0,0,0);
	}

//	virtual bool bounceOffWalls(float left, float top, float right, float bottom, float damping = .3) {
//		if(bFree){
//			return false;
//		}
//
//		bool collision = false;
//
//		if (x > right) {
//			x = right - 1;
//			xv *= -1;
//			collision = true;
//		} else if (x < left) {
//			x = left + 1;
//			xv *= -1;
//			collision = true;
//		}
//		if (y > bottom) {
//			y = bottom -1;
//			yv *= -1;
//			collision = true;
//		} else if (y < top) {
//			y = top +1;
//			yv *= -1;
//			collision = true;
//		}
//
//		if (collision == true) {
//			xv *= damping;
//			yv *= damping;
//		}
//		return collision;
//	}

	void addDampingForce(float damping = .01) {
		force = force - vel * damping;
	}

	virtual void draw(float grey = 255) {
		if(bFree){
			return;
		}
		ofSetColor(grey,grey,grey,alpha);//TODO include z
		ofCircle(x,y,radius);
	}

	virtual void drawVertex(){
		if(bFree){
			return;
		}
		glPointSize(ofRandom(1));
		glBegin(GL_POINTS);
		ofSetColor(255,255,255,alpha);
		glVertex3f(x,y,z);
		glEnd();
	}
};

inline float InvSqrt(float x) {
	float xhalf = 0.5f * x;
	int i = *(int*) &x; // store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
	x = *(float*) &i; // convert new bits into float
	x = x * (1.5f - xhalf * x * x); // One round of Newton's method
	return x;
}
