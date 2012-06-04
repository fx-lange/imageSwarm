#include "DataSet.h"

DataSet::DataSet() {

}

DataSet::~DataSet() {
}

int DataSet::loadImage(ofImage image){
	int width = image.getWidth();
	int height = image.getHeight();
	loaded = 0;

	int stepSize = 2;
	float scaleImage = 2.f;

	for(int x=0;x<width;x+=stepSize){
		for(int y=0;y<height;y+=stepSize){
			ofColor c = image.getColor(x,y);
			if(c.getBrightness() < 100){
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

void DataSet::pixelsToParticles(SwarmParticleSystem * ps){
	for(int i=0;i<size();++i){
		SwarmParticle * swarmPartile = ps->getNext();
		PixelData * p = pixels[i];
		swarmPartile->origin.set(p->x,p->y,0);
		swarmPartile->state = PARTICLE_ORIGIN;
	}
}

