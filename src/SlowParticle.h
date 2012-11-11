#pragma once

#include "Particle.h"

class SlowParticle: public SwarmParticle {
private:
	float slowMo;
public:

	SlowParticle(float _x = 0, float _y = 0, float _xv = 0, float _yv = 0)
		:SwarmParticle(_x,_y,_xv,_yv){
		slowMo = 1.f;
	}

	virtual ~SlowParticle() {
	}

	virtual void slowMe(){
		slowMo *= settings->slowForce;
	}

//	virtual void updatePosition(float timeStep){
//		SwarmParticle::updatePosition(timeStep*slowMo);
////		if(slowMo < 0.99f){ true??!?!?!
////			return;
////		}
//	}

//	virtual void resetForces(){
//		SwarmParticle::resetForce();
//		slowMo *= settings->slowReset;
//		slowMo = slowMo > 1.f ? 1.f : slowMo;
//	}

	virtual void drawVertex(){
		SwarmParticle::drawVertex();
	}
};
