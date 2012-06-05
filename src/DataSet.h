#ifndef DATASET_H_
#define DATASET_H_

#include "ofMain.h"
#include "ParticleSystem.h"

/** TODO
 * dependent behaviour between datasets
 * size depent particle amount
 */

struct PixelData{
	float x,y;
	ofColor c;
	SwarmParticle * particle;
};

class DataSet {
protected:
	vector<PixelData*> pixels;
	int loaded;

public:
	DataSet();

	virtual ~DataSet();

	int size(){
		return pixels.size();
	}

	int loadImage(ofImage image, int stepSize, bool white = false);

	void pixelsToParticles(SwarmParticleSystem * ps);

	void setOriginForceActive(bool active);
	void scaleOrigins(float scaleX, float scaleY);
	void translateOrigins(float transX, float transY, float transZ);

	void freeParticles();
};

#endif /* DATASET_H_ */
