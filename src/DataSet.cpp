#include "DataSet.h"

DataSet::DataSet() {
}

DataSet::~DataSet() {
}

int DataSet::loadImage(string filename, int stepSize, bool white) {
	ofImage image;
	image.loadImage(filename);
	image.setImageType(OF_IMAGE_COLOR);
	used = false;
	return loadImage(image, stepSize, white);
}

int DataSet::loadImage(ofImage & image, int stepSize, bool white) {
	int width = image.getWidth();
	int height = image.getHeight();

	loaded = 0;

	float minX, minY, maxX, maxY;
	minX = width;
	minY = height;
	maxX = 0;
	maxY = 0;

	for (int x = 0; x < width; x += stepSize) {
		for (int y = 0; y < height; y += stepSize) {
			ofColor c = image.getColor(x, y);
			if ((white && c.getBrightness() > 1)
					|| (!white && c.getBrightness() < 100)) {
				PixelData * p = new PixelData();
				p->x = x;
				p->y = y;
				p->c = c;
				pixels.push_back(p);
				++loaded;

				if (x > maxX)
					maxX = x;
				if (x < minX)
					minX = x;
				if (y > maxY)
					maxY = y;
				if (y < minY)
					minY = y;
			}
		}
	}
	boundingBox.set(minX, minY, maxX - minX, maxY - minY);
	return loaded;
}

int DataSet::pixelsToParticles(SwarmParticleSystem * ps, bool notFree) {
	for (int i = 0; i < size(); ++i) {
		SwarmParticle * swarmParticle = ps->getNextUnused(notFree);
		PixelData * p = pixels[i];
		swarmParticle->origin.set(p->x, p->y, 0);
		swarmParticle->state = PARTICLE_ORIGIN;
		swarmParticle->c = p->c;
		p->particle = swarmParticle;
		p->used = true;
	}
	used = true;
	return size();
}

void DataSet::checkBorders(SwarmParticleSystem * ps, float minX, float minY, float maxX, float maxY){
	if(!used){
		return;
	}
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		bool inside = true;
		if (p->x < minX || p->x > maxX || p->y < minY
				|| p->y > maxY ) {
			inside = false;
		}

		if(p->used && !inside){
			//raus
			p->particle->setUsed(false);
			p->particle->state = PARTICLE_ZLAYER;
			p->particle = NULL;
			p->used = false;

		}else if(!p->used && inside){
			//rein
			SwarmParticle * swarmParticle = ps->getNextUnused();
			PixelData * p = pixels[i];
			swarmParticle->origin.set(p->x, p->y, 0);
			swarmParticle->state = PARTICLE_ORIGIN;
			swarmParticle->c = p->c;
			p->particle = swarmParticle;
			p->used = true;
		}
	}
}

void DataSet::setOriginForceActive(bool active) {
	if (!used) {
		return;
	}
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
	if (!used) {
		return;
	}
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->x *= scaleX;
		p->y *= scaleY;
		p->particle->origin.x = p->x;
		p->particle->origin.y = p->y;
	}
	boundingBox.width *= scaleX;
	boundingBox.height *= scaleY;
}

void DataSet::translateOrigins(float transX, float transY, float transZ) {
	if (!used) {
		return;
	}
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->x += transX;
		p->y += transY;
		if(!p->used)
			continue;
		p->particle->origin.x = p->x;
		p->particle->origin.y = p->y;
		p->particle->origin.z += transZ;
	}
	boundingBox.x += transX;
	boundingBox.y += transY;
}

void DataSet::moveOriginsBBSize(DataSet & other, float leftright, float updown,
		float offSetX, float offSetY) {
	if (!used) {
		return;
	}
	float transX = leftright * other.boundingBox.width
			+ ofSign(leftright) * offSetX;
	float transY = updown * other.boundingBox.height + ofSign(updown) * offSetY;
	cout << "translate: " << transX << " / " << transY << endl;
	translateOrigins(transX, transY);
}

void DataSet::scaleOriginsFromCenter(float scaleX, float scaleY) {
	if (!used) {
		return;
	}
	ofPoint center(boundingBox.width / 2 + boundingBox.x,
			boundingBox.height / 2 + boundingBox.y);
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
	diff = center - ofVec3f(boundingBox.x, boundingBox.y);
	diff.x *= scaleX;
	diff.y *= scaleY;
	boundingBox.x = center.x + diff.x;
	boundingBox.y = center.y + diff.y;
	boundingBox.width *= scaleX;
	boundingBox.height *= scaleY;
}

int DataSet::freeParticles(int freeModulo) {
	if (!used) {
		return 0;
	}
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->particle->state = PARTICLE_ZLAYER;
		p->particle->setUsed(false);
		if( i%2 == 0 && freeModulo % 2 == 0)
			p->particle->setFree(true,true);
		if( i%3 == 0 && freeModulo % 3 == 0)
			p->particle->setFree(true,true);
//		p->c.set(255,255,255);
		p->particle = NULL;
		p->used = false;
	}
	used = false;
	return size();
}

