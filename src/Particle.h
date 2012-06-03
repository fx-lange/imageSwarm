#pragma once

#include "ofMain.h"
#include "ofGraphics.h"
#include "ofPoint.h"

class SwarmParticle : public ofPoint {
public:
	ofVec3f vel;
	ofVec3f steer;
	ofPoint origin;

	float radius;
	float alpha;

	SwarmParticle(float _x = 0, float _y = 0, float _xv = 0, float _yv = 0);

	virtual ~SwarmParticle(){ }

	virtual bool isFree(){
		return bFree;
	}

	virtual bool ignoresForces(){
		return bIgnoreForce;
	}

	virtual void setFree(bool free);

	//gravity force to additional defined origin
	//TODO faster(invSQRT)
	//TODO smaller force, effect = (length/size)*force with size ≃ windowsize
	//		addOriginForce(force,size)
	virtual void addOriginForce(float scale);

	virtual void updatePosition(float timeStep);

	void resetForce();

	void addDampingForce(float damping = .01);

	virtual void draw(float grey = 255);

	virtual void drawVertex();


protected:

	bool bFree;
	bool bIgnoreForce;
	bool bKillSoft;

};

inline float InvSqrt(float x) {
	float xhalf = 0.5f * x;
	int i = *(int*) &x; // store floating-point bits in integer
	i = 0x5f3759d5 - (i >> 1); // initial guess for Newton's method
	x = *(float*) &i; // convert new bits into float
	x = x * (1.5f - xhalf * x * x); // One round of Newton's method
	return x;
}
