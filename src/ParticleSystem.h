#pragma once

#include "Particle.h"

//#define DRAW_FORCES
#define USE_INVSQRT

class SwarmParticleSystem {
protected:
	int index;

	float timeStep;

	vector<vector<SwarmParticle*> > bins;
	int width, height, k, xBins, yBins, binSize;

public:

    int nFree;

	vector<SwarmParticle*> particles;
	SwarmParticleSystem();
	~SwarmParticleSystem(){}

	void setup(int width, int height, int k);
	void setTimeStep(float timeStep);

	void add(SwarmParticle * particle);
	vector<SwarmParticle*> getNeighbors(SwarmParticle& particle, float radius);
	vector<SwarmParticle*> getNeighbors(float x, float y, float radius);
	vector<SwarmParticle*> getRegion(unsigned minX, unsigned minY, unsigned maxX,
			unsigned maxY);
	unsigned size() const;
	SwarmParticle& operator[](unsigned i);
	SwarmParticle * getNext();

	void setupForces();
	void addRepulsionForce(const ofPoint& p, float radius, float scale);
	void addRepulsionForce(float x, float y, float z, float radius, float scale);
	void addAttractionForce(const ofPoint& p, float radius,
			float scale);
	void addAttractionForce(float x, float y, float z, float radius, float scale);
	void addForce(const  ofPoint& p, float radius, float scale);
	void addForce(float x, float y, float z, float radius, float scale);
	virtual void update(bool ignoreFree = true);

	virtual void draw(bool circle=false);

	void freeAllParticles();
};

//class ParticleSystemQ3 : public ParticleSystem{
//public:
//	virtual void update(float radius = -10){
//		int n = particles.size();
//		int iFree=0;
//		for (int i = 0; i < n; i++){
//			ParticleQ3 * pQ3 = (ParticleQ3*)particles[i];
//			pQ3->updatePosition(timeStep);
//			if(radius>-10)
//				pQ3->d = pQ3->initD = radius*2;
//			if(pQ3->bFree)
//				iFree++;
//		}
//		if(iFree < 1000){
//			ofLog(OF_LOG_WARNING,"PARTICLE LEAK - FREE ALL!");
//			freeAllParticles();
//		}
//	}
//
//	void boost(float input,float percent) {
//		int n = particles.size();
//		for (int i = 0; i < n; i++){
//			ParticleQ3 * pQ3 = (ParticleQ3*)particles[i];
//			if(pQ3->bFree)
//				continue;
//			float percentInput = (percent / 100) * input;
//			float randomBoost = ofRandom(-percentInput,percentInput)+input;
//			pQ3->boost(randomBoost);
//		}
//	}
//
//	virtual void draw(float grey = 255){
//		int n = particles.size();
//		for (int i = 0; i < n; i++){
//			ParticleQ3  * pQ3 = (ParticleQ3*)particles[i];
//			pQ3->draw(grey);
//		}
//	}
//};

//class ParticleSystemLine : public ParticleSystem{
//public:
//	virtual void update(float radius = -10){
//		int n = particles.size();
//		int iFree=0;
//		for (int i = 0; i < n; i++){
//			ParticleQ3 * pQ3 = (ParticleQ3*)particles[i];
//			pQ3->updatePosition(timeStep);
//			if(radius>-10)
//				pQ3->d = pQ3->initD = radius*2;
//			if(pQ3->bFree)
//				iFree++;
//		}
//		if(iFree < 1000){
//			cout << "WARNING - PARTICLE LEAK - FREE ALL!" << endl;
//			freeAllParticles();
//		}
//	}
//
//	void boost(float input,float percent) {
//		int n = particles.size();
//		for (int i = 0; i < n; i++){
//			ParticleQ3 * pQ3 = (ParticleQ3*)particles[i];
//			if(pQ3->bFree)
//				continue;
//			float percentInput = (percent / 100) * input;
//			float randomBoost = ofRandom(-percentInput,percentInput)+input;
//			pQ3->boost(randomBoost);
//		}
//	}
//
//	virtual void draw(float grey = 255){
//		int n = particles.size();
//		for (int i = 0; i < n; i++){
//			ParticleQ3  * pQ3 = (ParticleQ3*)particles[i];
//			pQ3->draw(grey);
//		}
//	}
//};

