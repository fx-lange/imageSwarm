#include "DataSet.h"

DataSet::DataSet() {
}

DataSet::~DataSet() {
}

int DataSet::loadImage(string filename, int stepSize, bool white) {
	ofImage image;
	image.loadImage(filename);
	image.setImageType(OF_IMAGE_COLOR);
	isInUse = false;
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
			if ((white && c.getBrightness() > 200)
					|| (!white && c.getBrightness() < 50)) {
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

void DataSet::pixelsToParticles(SwarmParticleSystem * ps, bool notFree) {
	for (int i = 0; i < size(); ++i) {
		SwarmParticle * swarmParticle = ps->getNextUnused(notFree);
		PixelData * p = pixels[i];
		swarmParticle->origin.set(p->x, p->y, 0);
		swarmParticle->state = PARTICLE_ORIGIN;
		swarmParticle->c = p->c;
		p->particle = swarmParticle;
		p->hasParticle = true;
	}
	isInUse = true;
}

void DataSet::reuseDataSet(SwarmParticleSystem * ps, DataSet * reuseDataSet,int percent) {
	for (int i = 0; i < size(); ++i) {
		int dice = ofRandom(0,99);

		SwarmParticle * swarmParticle;
		if( dice < percent ){
			swarmParticle ; //TODO get from reusedataset
			bool bFound = false;
			for(int j=i;j<reuseDataSet->size()&&!bFound;++j){
				PixelData * reusePixel = reuseDataSet->getPixel(j);
				if(reusePixel->particle != NULL && !reusePixel->particle->isUsed()){
					swarmParticle = reusePixel->particle;
					ps->setParticleUsed(swarmParticle);
					bFound = true;
				}
			}

			if(!bFound){
				cout << "reuse failed" << endl;
				percent = 0;
				swarmParticle = ps->getNextUnused(true);
			}
		}else{
			swarmParticle = ps->getNextUnused(true);
		}
		PixelData * p = pixels[i];
		swarmParticle->origin.set(p->x, p->y, 0);
		swarmParticle->state = PARTICLE_ORIGIN;
		swarmParticle->c = p->c;
		p->particle = swarmParticle;
		p->hasParticle = true;
	}
	isInUse = true;
}

void DataSet::checkBorders(SwarmParticleSystem * ps, float minX, float minY, float maxX, float maxY){
	if(!isInUse){
		return;
	}
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		bool isInside = true;
		if (p->x < minX || p->x > maxX || p->y < minY
				|| p->y > maxY ) {
			isInside = false;
		}

		if(p->hasParticle && !isInside){
			//raus
			ps->passbackParticle(p->particle);
			p->particle = NULL;
			p->hasParticle = false;

		}else if(!p->hasParticle && isInside){
			//rein
			SwarmParticle * swarmParticle = ps->getNextUnused();
			PixelData * p = pixels[i];
			swarmParticle->origin.set(p->x, p->y, 0);
			swarmParticle->state = PARTICLE_ORIGIN;
			swarmParticle->c = p->c;
			p->particle = swarmParticle;
			p->hasParticle = true;
		}
	}
}

void DataSet::setOriginForceActive(bool active) {
	if (!isInUse) {
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
	if (!isInUse) {
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
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		p->x += transX;
		p->y += transY;
		if(!p->hasParticle)
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
	if (!isInUse) {
		return;
	}
	float transX = leftright * other.boundingBox.width
			+ ofSign(leftright) * offSetX;
	float transY = updown * other.boundingBox.height + ofSign(updown) * offSetY;
	cout << "translate: " << transX << " / " << transY << endl;
	translateOrigins(transX, transY);
}

void DataSet::scaleOriginsFromCenter(float scaleX, float scaleY) {
	if (!isInUse) {
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

void DataSet::freeParticles(SwarmParticleSystem * ps,int freeModulo) {
	if (!isInUse) {
		return;
	}
	for (int i = 0; i < size(); ++i) {
		PixelData * p = pixels[i];
		bool setFree = false;
		if( i%2 == 0 && freeModulo % 2 == 0)
			setFree = true;
		if( i%3 == 0 && freeModulo % 3 == 0)
			setFree = true;
//		p->c.set(255,255,255);
		ps->passbackParticle(p->particle,setFree);
//		p->particle = NULL; keep pointer for reuse possibility
		p->hasParticle = false;
	}
	isInUse = false;
}

