#pragma once

#include "ofMain.h"
#include "ofGraphics.h"
#include "ofPoint.h"

class SwarmParticle : public ofPoint {
public:
	float xv, yv;
	float xf, yf;

	float radius;
	float alpha;
	bool bFree;
	bool bNoForce;
	bool bKillSoft;

	ofPoint origin; //


	SwarmParticle(float _x = 0, float _y = 0, float _xv = 0, float _yv = 0) :
			ofPoint(_x,_y), xv(_xv), yv(_yv) {
		bFree = true;
		bNoForce = true;
		alpha = 0;
		radius = 2;
		origin.set(_x,_y);
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

	virtual void addOriginForce(float force){
		float xd = x - origin.x;
		float yd = y - origin.y;
		float length = xd * xd + yd * yd;
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
			float effect = length * force;
			xd *= effect;
			yd *= effect;
			xf += xd;
			yf += yd;
		}

		//TODO Kraft muss mit steigendem Abstand größer werden - nicht andersrum
	}

	virtual void updatePosition(float timeStep) {
		if(bFree)
			return;
		// f = ma, m = 1, f = a, v = int(a)
		xv += xf;
		yv += yf;
		x += xv * timeStep;
		y += yv * timeStep;

	}

	void resetForce() {
		xf = 0;
		yf = 0;
	}

	virtual bool bounceOffWalls(float left, float top, float right, float bottom, float damping = .3) {
		if(bFree){
			return false;
		}

		bool collision = false;

		if (x > right) {
			x = right - 1;
			xv *= -1;
			collision = true;
		} else if (x < left) {
			x = left + 1;
			xv *= -1;
			collision = true;
		}
		if (y > bottom) {
			y = bottom -1;
			yv *= -1;
			collision = true;
		} else if (y < top) {
			y = top +1;
			yv *= -1;
			collision = true;
		}

		if (collision == true) {
			xv *= damping;
			yv *= damping;
		}
		return collision;
	}

	void addDampingForce(float damping = .01) {
		xf = xf - xv * damping;
		yf = yf - yv * damping;
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
//		glPointSize(ofRandom(5));
		glPointSize(ofRandom(1));
		glBegin(GL_POINTS);
		ofSetColor(255,255,255,alpha);
		glVertex2f(x,y);
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
