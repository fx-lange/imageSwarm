/*
 * SlowParticleSystem.h
 *
 *  Created on: Nov 2, 2012
 *      Author: felix
 */

#ifndef SLOWPARTICLESYSTEM_H_
#define SLOWPARTICLESYSTEM_H_

#include "ParticleSystem.h"

class SlowParticleSystem : public SwarmParticleSystem{
public:
	void addSlow(float x, float y, float z, float radius);
};

#endif /* SLOWPARTICLESYSTEM_H_ */
