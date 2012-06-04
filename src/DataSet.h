#ifndef DATASET_H_
#define DATASET_H_

#include "ofMain.h"
#include "ParticleSystem.h"

struct PixelData{
	float x,y;
	ofColor c;
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

	int loadImage(ofImage image);

	void pixelsToParticles(SwarmParticleSystem * ps);
};

#endif /* DATASET_H_ */
