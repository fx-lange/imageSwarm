#include "DataSet.h"

DataSet::DataSet() {
}

DataSet::~DataSet() {
}

int DataSet::loadImage(string filename,int stepSize,bool white){
	ofImage image;
	image.loadImage(filename);
	image.setImageType(OF_IMAGE_GRAYSCALE);
	return loadImage(image,stepSize,white);
}

int DataSet::loadImage(ofImage & image, int stepSize, bool white) {
	int width = image.getWidth();
	int height = image.getHeight();
	boundingBox.set(0,0,width,height);
	loaded = 0;

	float scaleImage = 2.f;

	for (int x = 0; x < width; x += stepSize) {
		for (int y = 0; y < height; y += stepSize) {
			ofColor c = image.getColor(x, y);
			if ((white && c.getBrightness() > 100)
					|| (!white && c.getBrightness() < 100)) {
				PixelData * p = new PixelData();
				p->x = x;
				p->y = y;
				p->c = c;
				pixels.push_back(p);
				++loaded;
			}
		}
	}
	return loaded;
}

int DataSet::pixelsToParticles(SwarmParticleSystem * ps) {
	for (int i = 0; i < size(); ++i) {
		SwarmParticle * swarmParticle = ps->getNextUnused();
		PixelData * p = pixels[i];
		swarmParticle->origin.set(p->x, p->y, 0);
		swarmParticle->state = PARTICLE_ORIGIN;
		p->particle = swarmParticle;
	}
	return size();
}

void DataSet::setOriginForceActive(bool active) {
	particleState state = PARTICLE_FREE;
	if (active) {
		state = PARTICLE_ORIGIN;
	}
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->particle->state = state;
	}
}

void DataSet::scaleOrigins(float scaleX, float scaleY) {
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->particle->origin.x *= scaleX;
		p->particle->origin.y *= scaleY;
		p->particle->origin.x += 100;
		p->particle->origin.y += 100;
	}
	boundingBox.width *= scaleX;
	boundingBox.height *= scaleY;
}

void DataSet::translateOrigins(float transX, float transY, float transZ) {
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->particle->origin.x += transX;
		p->particle->origin.y += transY;
		p->particle->origin.z += transZ;
	}
	boundingBox.x += transX;
	boundingBox.y += transY;
}

void DataSet::moveOriginsBBSize(float leftright, float updown, float offSetX, float offSetY){
	float transX = leftright * boundingBox.width + ofSign(leftright) * offSetX;
	float transY = updown * boundingBox.height + ofSign(updown) * offSetY;
	translateOrigins(transX,transY);
}

void DataSet::scaleOriginsFromCenter(float scaleX,float scaleY){
	ofPoint center(boundingBox.width/2+boundingBox.x,boundingBox.height/2+boundingBox.y);
	ofVec3f diff;
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		ofVec3f & origin = p->particle->origin;
		diff = center - origin;
		diff.x *= scaleX;
		diff.y *= scaleY;
		origin = center + diff;
	}

	//scale boundingbox from center
	diff = center - ofVec3f(boundingBox.x,boundingBox.y);
	diff.x *= scaleX;
	diff.y *= scaleY;
	boundingBox.x = center.x + diff.x;
	boundingBox.y = center.y + diff.y;
	boundingBox.width *= scaleX;
	boundingBox.height *= scaleY;
}

int DataSet::freeParticles() {
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->particle->state = PARTICLE_ZLAYER;
		p->particle->setUsed(false);
		p->particle = NULL;
	}
	return size();
}

