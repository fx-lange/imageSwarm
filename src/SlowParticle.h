#pragma once

#include "Particle.h"

class SlowParticle: public SwarmParticle {

public:

	SlowParticle(float _x = 0, float _y = 0, float _xv = 0, float _yv = 0)
		:SwarmParticle(_x,_y,_xv,_yv){
		slowMo = 0.f;
	}

	virtual ~SlowParticle() {
	}
	float slowMo;

	virtual void drawVertex(){
		SwarmParticle::drawVertex();
	}
};
