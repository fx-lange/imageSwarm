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
		SwarmParticle * swarmParticle = ps->getNextUnused();
		PixelData * p = pixels[i];
		swarmParticle->origin.set(p->x,p->y,0);
		swarmParticle->state = PARTICLE_ORIGIN;
		p->particle = swarmParticle;
	}
}

void DataSet::setOriginForceActive(bool active){
	particleState state = PARTICLE_FREE;
	if(active){
		state = PARTICLE_ORIGIN;
	}
	for(int i=0;i<size();++i){
		PixelData * p = pixels[i];
		p->particle->state = state;
	}
}

void DataSet::scaleOrigins(float scaleX, float scaleY){
	for(int i=0;i<size();++i){
		PixelData * p = pixels[i];
		p->particle->origin.x *= scaleX;
		p->particle->origin.y *= scaleY;
	}
}

void DataSet::translateOrigins(float transX, float transY){
	for(int i=0;i<size();++i){
		PixelData * p = pixels[i];
		p->particle->origin.x += transX;
		p->particle->origin.y += transY;
	}
}

void DataSet::freeParticles(){
	for(int i=0;i<size();++i){
		PixelData * p = pixels[i];
		p->particle->state = PARTICLE_FREE;
		p->particle->setUsed(false);
		p->particle = NULL;
	}
}

