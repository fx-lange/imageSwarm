#ifndef DATASET_H_
#define DATASET_H_

#include "ofMain.h"
#include "ParticleSystem.h"

/** TODO
 * dependent behaviour between datasets
 * size depent particle amount
 *
 * improve and rename PixelData and DataSet ...
 */

struct PixelData {
	bool hasParticle;
	float x, y;
	ofColor c;
	SwarmParticle * particle;
};

class DataSet {
protected:
	ofRectangle boundingBox;
	vector<PixelData*> pixels;
	int loaded;
	int isInUse;

public:
	DataSet();

	virtual ~DataSet();

	int size() {
		return pixels.size();
	}

	int loadImage(string filename, int stepSize, bool white = false);
	int loadImage(ofImage & image, int stepSize, bool white = false);

	void pixelsToParticles(SwarmParticleSystem * ps, bool onlyActive = true);
	void reuseDataSet(SwarmParticleSystem * ps, DataSet * reuseDataSet, int percent);

	PixelData * getPixel(int idx){
		if(idx >= (int)pixels.size()){
			return NULL;
		}
		return pixels[idx];
	}

	void checkBorders(SwarmParticleSystem * ps, float minX, float minY, float maxX, float maxY);
	void setOriginForceActive(bool active);
	void scaleOrigins(float scaleX, float scaleY);
	void translateOrigins(float transX, float transY, float transZ = 0);
	void moveOriginsBBSize(DataSet & other, float leftright, float updown,
			float offSetX = 0, float offSetY = 0);
	void scaleOriginsFromCenter(float scaleX, float scaleY);

	void freeParticles(SwarmParticleSystem * ps, int freeModulo = 1);
};

#endif /* DATASET_H_ */
