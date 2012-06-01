#include "ParticleSystem.h"

SwarmParticleSystem::SwarmParticleSystem() :
		timeStep(1) {
}

void SwarmParticleSystem::setup(int width, int height, int k) {
	this->width = width;
	this->height = height;
	this->k = k;
	binSize = 1 << k;
	xBins = (int) ceilf((float) width / (float) binSize);
	yBins = (int) ceilf((float) height / (float) binSize);
	bins.resize(xBins * yBins);
	index = 0;
}

void SwarmParticleSystem::setTimeStep(float timeStep) {
	this->timeStep = timeStep;
}

void SwarmParticleSystem::add(SwarmParticle * particle) {
	particles.push_back(particle);
}

unsigned SwarmParticleSystem::size() const {
	return particles.size();
}

SwarmParticle& SwarmParticleSystem::operator[](unsigned i) {
	SwarmParticle * p = particles[i];
	return (*p);
}

SwarmParticle * SwarmParticleSystem::getNext(){
	SwarmParticle * p = particles[index];
	index = (index+1) % (particles.size());
	while(!p->bFree){
		p = particles[index];
		index = (index+1) % (particles.size());
	}
	p->setFree(false);
	return p;
}

vector<SwarmParticle*> SwarmParticleSystem::getNeighbors(SwarmParticle& particle,
		float radius) {
	return getNeighbors(particle.x, particle.y, radius);
}

vector<SwarmParticle*> SwarmParticleSystem::getNeighbors(float x, float y, float radius) {
	vector<SwarmParticle*> region = getRegion((int) (x - radius),
	(int) (y - radius),
	(int) (x + radius),
	(int) (y + radius));
	vector<SwarmParticle*> neighbors;
	int n = region.size();
	float xd, yd, rsq, maxrsq;
	maxrsq = radius * radius;
	for(int i = 0; i < n; i++) {
		SwarmParticle& cur = *region[i];
		xd = cur.x - x;
		yd = cur.y - y;
		rsq = xd * xd + yd * yd;
		if(rsq < maxrsq)
		neighbors.push_back(region[i]);
	}
	return neighbors;
}

vector<SwarmParticle*> SwarmParticleSystem::getRegion(unsigned minX, unsigned minY,
		unsigned maxX, unsigned maxY) {
	vector<SwarmParticle*> region;
	back_insert_iterator<vector<SwarmParticle*> > back = back_inserter(region);
	unsigned minXBin = minX >> k;
	unsigned maxXBin = maxX >> k;
	unsigned minYBin = minY >> k;
	unsigned maxYBin = maxY >> k;
	maxXBin++;
	maxYBin++;
	if (maxXBin > xBins)
		maxXBin = xBins;
	if (maxYBin > yBins)
		maxYBin = yBins;
	for (int y = minYBin; y < maxYBin; y++) {
		for (int x = minXBin; x < maxXBin; x++) {
			vector<SwarmParticle*>& cur = bins[y * xBins + x];
			copy(cur.begin(), cur.end(), back);
		}
	}
	return region;
}

void SwarmParticleSystem::setupForces() {
	int n = bins.size();
	for (int i = 0; i < n; i++) {
		bins[i].clear();
	}
	n = particles.size();
	unsigned xBin, yBin, bin;
	for (int i = 0; i < n; i++) {
		SwarmParticle& cur = *(particles[i]);
		cur.resetForce();
		xBin = ((unsigned) cur.x) >> k;
		yBin = ((unsigned) cur.y) >> k;
		bin = yBin * xBins + xBin;
		if (xBin < xBins && yBin < yBins)
			bins[bin].push_back(&cur);
	}
}

void SwarmParticleSystem::addRepulsionForce(const ofPoint& p, float radius,
		float scale) {
	addRepulsionForce(p.x, p.y, radius, scale);
}

void SwarmParticleSystem::addRepulsionForce(float x, float y, float radius,
		float scale) {
	addForce(x, y, radius, scale);
}

void SwarmParticleSystem::addAttractionForce(const ofPoint& p, float radius,
		float scale) {
	addAttractionForce(p.x, p.y, radius, scale);
}

void SwarmParticleSystem::addAttractionForce(float x, float y, float radius,
		float scale) {
	addForce(x, y, radius, -scale);
}

void SwarmParticleSystem::addForce(const ofPoint& p, float radius,
		float scale) {
	addForce(p.x, p.y, radius, -scale);
}

void SwarmParticleSystem::addForce(float targetX, float targetY, float radius,
		float scale) {
	float minX = targetX - radius;
	float minY = targetY - radius;
	float maxX = targetX + radius;
	float maxY = targetY + radius;
	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	unsigned minXBin = ((unsigned) minX) >> k;
	unsigned minYBin = ((unsigned) minY) >> k;
	unsigned maxXBin = ((unsigned) maxX) >> k;
	unsigned maxYBin = ((unsigned) maxY) >> k;
	maxXBin++;
	maxYBin++;
	if (maxXBin > xBins)
		maxXBin = xBins;
	if (maxYBin > yBins)
		maxYBin = yBins;
	float xd, yd, length, maxrsq;
#ifdef USE_INVSQRT
	float xhalf;
	int lengthi;
#else
	float effect;
#endif
	maxrsq = radius * radius;
	for (int y = minYBin; y < maxYBin; y++) {
		for (int x = minXBin; x < maxXBin; x++) {
			vector<SwarmParticle*>& curBin = bins[y * xBins + x];
			int n = curBin.size();
			for (int i = 0; i < n; i++) {
				SwarmParticle& curParticle = *(curBin[i]);
				if(curParticle.bFree || curParticle.bNoForce){
					continue;
				}
				xd = curParticle.x - targetX;
				yd = curParticle.y - targetY;
				length = xd * xd + yd * yd;
				if (length > 0 && length < maxrsq * 2) {
#ifdef DRAW_FORCES
					glVertex2f(targetX, targetY);
					glVertex2f(curParticle.x, curParticle.y);
#endif
				}
				if (length > 0 && length < maxrsq) {
//						glVertex2f(targetX, targetY);
//						glVertex2f(curParticle.x, curParticle.y);//REVISIT wieso hier malen?
#ifdef USE_INVSQRT
					xhalf = 0.5f * length;
					lengthi = *(int*) &length;
					lengthi = 0x5f3759df - (lengthi >> 1);
					length = *(float*) &lengthi;
					length *= 1.5f - xhalf * length * length;
					xd *= length;
					yd *= length;
					length *= radius;
					length = 1 / length;
					length = (1 - length);
					length *= scale;
					xd *= length;
					yd *= length;
					curParticle.xf += xd;
					curParticle.yf += yd;
#else
					length = sqrtf(length);
					xd /= length;
					yd /= length;
					effect = (1 - (length / radius)) * scale;
					curParticle.xf += xd * effect;
					curParticle.yf += yd * effect;
#endif
				}
			}
		}
	}
}

void SwarmParticleSystem::addDirectedForce(float targetX, float targetY, float radius, float scale, const ofVec3f & normDirection) {
	float minX = targetX - radius;
	float minY = targetY - radius;
	float maxX = targetX + radius;
	float maxY = targetY + radius;
	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	unsigned minXBin = ((unsigned) minX) >> k;
	unsigned minYBin = ((unsigned) minY) >> k;
	unsigned maxXBin = ((unsigned) maxX) >> k;
	unsigned maxYBin = ((unsigned) maxY) >> k;
	maxXBin++;
	maxYBin++;
	if (maxXBin > xBins)
		maxXBin = xBins;
	if (maxYBin > yBins)
		maxYBin = yBins;
	float xd, yd, length, maxrsq;
#ifdef USE_INVSQRT
	float xhalf;
	int lengthi;
#else
	float effect;
#endif
	maxrsq = radius * radius;
	for (int y = minYBin; y < maxYBin; y++) {
		for (int x = minXBin; x < maxXBin; x++) {
			vector<SwarmParticle*>& curBin = bins[y * xBins + x];
			int n = curBin.size();
			for (int i = 0; i < n; i++) {
				SwarmParticle& curParticle = *(curBin[i]);
				xd = curParticle.x - targetX;
				yd = curParticle.y - targetY;
				length = xd * xd + yd * yd;
				if (length > 0 && length < maxrsq * 2) {
#ifdef DRAW_FORCES
					glVertex2f(targetX, targetY);
					glVertex2f(curParticle.x, curParticle.y);
#endif
				}
				if (length > 0 && length < maxrsq) {
					#ifdef DRAW_FORCES
						glVertex2f(targetX, targetY);
						glVertex2f(curParticle.x, curParticle.y);
					#endif
#ifdef USE_INVSQRT
					xhalf = 0.5f * length;
					lengthi = *(int*) &length;
					lengthi = 0x5f3759df - (lengthi >> 1);
					length = *(float*) &lengthi;
					length *= 1.5f - xhalf * length * length;
//					xd *= length;
//					yd *= length;
//					xd = direction.x / 100;
//					yd = direction.y / 100;
					xd = normDirection.x;
					yd = normDirection.y;
					length *= radius;
					length = 1 / length;
					length = (1 - length);
					length *= scale;
					xd *= length;
					yd *= length;
					curParticle.xf += xd;
					curParticle.yf += yd;
#else
					length = sqrtf(length);
//					xd /= length;
//					yd /= length;
					xd = 0.1;
					yd = 0;
					effect = (1 - (length / radius)) * scale;
					curParticle.xf += xd * effect;
					curParticle.yf += yd * effect;
#endif
				}
			}
		}
	}
}


void SwarmParticleSystem::update(bool ignoreFree) {
	int iFree = 0;
	int n = particles.size();
	for (int i = 0; i < n; i++){
		SwarmParticle * p = particles[i];
		p->updatePosition(timeStep);
		if(p->bFree){
			++iFree;
		}
	}
	if(!ignoreFree && iFree < 5000){
		ofLog(OF_LOG_WARNING, "WARNING - PARTICLE LEAK - FREE ALL!");
		freeAllParticles();
	}
	nFree = iFree;
}


void SwarmParticleSystem::draw(bool circle) {
	int n = particles.size();
	for (int i = 0; i < n; i++){
		if(circle){
			particles[i]->draw();
		}else{
			particles[i]->drawVertex();
		}
	}
}

void SwarmParticleSystem::freeAllParticles(){
	int n = particles.size();
	for (int i = 0; i < n; i++){
		SwarmParticle * p = particles[i];
		p->bFree = true;
	}
}
