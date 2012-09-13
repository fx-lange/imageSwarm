#pragma once

#include "Particle.h"
#include "FlowField.h"

#define USE_INVSQRT

class SwarmParticleSystem {
protected:

	float timeStep;

	vector<vector<SwarmParticle*> > bins;
	unsigned int width, height, binPower, xBins, yBins, binSize;

public:
	int index;

	int nActive;
	int nUsed;
	void printNumbers();

	vector<SwarmParticle*> particles;
	SwarmParticleSystem();
	virtual ~SwarmParticleSystem() {
	}

	void setup(int width, int height, int binPower);
	void setTimeStep(float timeStep);

	void add(SwarmParticle * particle);
	vector<SwarmParticle*> getNeighbors(SwarmParticle& particle, float radius);
	vector<SwarmParticle*> getNeighbors(float x, float y, float radius);
	vector<SwarmParticle*> getRegion(unsigned minX, unsigned minY,
			unsigned maxX, unsigned maxY);
	unsigned size() const;

	SwarmParticle& operator[](unsigned i);

	SwarmParticle * getNextFree();
	SwarmParticle * getNextUnused(bool notFree = false);
	void passbackParticle(SwarmParticle * p,bool setFree = false);
	void setParticleUsed(SwarmParticle * p);

	void setupForces();

	void addRepulsionForce(const ofPoint& p, float radius, float scale);
	void addRepulsionForce(float x, float y, float z, float radius,
			float scale);
	void addAttractionForce(const ofPoint& p, float radius, float scale);
	void addAttractionForce(float x, float y, float z, float radius,
			float scale);
	void addForce(const ofPoint& p, float radius, float scale);
	void addForce(float x, float y, float z, float radius, float scale);

	virtual void update(bool ignoreFree = true);

	virtual void draw(bool circle = false);

	void freeAllParticles();
};
