#include "ParticleSystem.h"

SwarmParticleSystem::SwarmParticleSystem() :
		timeStep(1) {
}

void SwarmParticleSystem::setup(int width, int height, int binPower) {
	this->width = width;
	this->height = height;
	this->binPower = binPower;
	binSize = 1 << binPower;
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
	while(!p->isFree()){
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
	unsigned minXBin = minX >> binPower;
	unsigned maxXBin = maxX >> binPower;
	unsigned minYBin = minY >> binPower;
	unsigned maxYBin = maxY >> binPower;
	maxXBin++;
	maxYBin++;
	if (maxXBin > xBins)
		maxXBin = xBins;
	if (maxYBin > yBins)
		maxYBin = yBins;
	for (unsigned int y = minYBin; y < maxYBin; y++) {
		for (unsigned int x = minXBin; x < maxXBin; x++) {
			vector<SwarmParticle*>& cur = bins[y * xBins + x];
			copy(cur.begin(), cur.end(), back);
		}
	}
	return region;
}

void SwarmParticleSystem::setupForces() {
	for (unsigned int i = 0; i < bins.size(); i++) {
		bins[i].clear();
	}

	unsigned xBin, yBin, bin;
	for (unsigned int i = 0; i <  particles.size(); i++) {
		SwarmParticle& cur = *(particles[i]);
		cur.resetForce();
		xBin = ((unsigned) cur.x) >> binPower;
		yBin = ((unsigned) cur.y) >> binPower;
		bin = yBin * xBins + xBin;
		if (xBin < xBins && yBin < yBins)
			bins[bin].push_back(&cur);
	}
}

void SwarmParticleSystem::addRepulsionForce(const ofPoint& p, float radius,
		float scale) {
	addRepulsionForce(p.x, p.y, p.z, radius, scale);
}

void SwarmParticleSystem::addRepulsionForce(float x, float y, float z, float radius,
		float scale) {
	addForce(x, y, z, radius, scale);
}

void SwarmParticleSystem::addAttractionForce(const ofPoint& p, float radius,
		float scale) {
	addAttractionForce(p.x, p.y, p.z, radius, scale);
}

void SwarmParticleSystem::addAttractionForce(float x, float y, float z, float radius,
		float scale) {
	addForce(x, y, z, radius, -scale);
}

void SwarmParticleSystem::addForce(const ofPoint& p, float radius,
		float scale) {
	addForce(p.x, p.y, p.z, radius, -scale);
}

void SwarmParticleSystem::addForce(float targetX, float targetY, float targetZ, float radius,
		float scale) {
	float minX = targetX - radius;
	float minY = targetY - radius;
	float maxX = targetX + radius;
	float maxY = targetY + radius;
	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	unsigned minXBin = ((unsigned) minX) >> binPower;
	unsigned minYBin = ((unsigned) minY) >> binPower;
	unsigned maxXBin = ((unsigned) maxX) >> binPower;
	unsigned maxYBin = ((unsigned) maxY) >> binPower;
	maxXBin++;
	maxYBin++;
	if (maxXBin > xBins)
		maxXBin = xBins;
	if (maxYBin > yBins)
		maxYBin = yBins;
	float xd, yd, zd, length, maxrsq;
#ifdef USE_INVSQRT
	float xhalf;
	int lengthi;
#else
	float effect;
#endif
	maxrsq = radius * radius;
	for (unsigned int y = minYBin; y < maxYBin; y++) {
		for (unsigned int x = minXBin; x < maxXBin; x++) {
			vector<SwarmParticle*>& curBin = bins[y * xBins + x];
			int n = curBin.size();
			for (int i = 0; i < n; i++) {
				SwarmParticle& curParticle = *(curBin[i]);
				if(curParticle.isFree() || curParticle.ignoresForces()){
					continue;
				}
				xd = curParticle.x - targetX;
				yd = curParticle.y - targetY;
				zd = curParticle.z - targetZ;
				length = xd * xd + yd * yd + zd * zd;
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
					zd *= length;
					length *= radius;
					length = 1 / length;
					length = (1 - length);
					length *= scale;
					xd *= length;
					yd *= length;
					zd *= length;
					curParticle.acc.x += xd;
					curParticle.acc.y += yd;
					curParticle.acc.z += zd;
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


void SwarmParticleSystem::update(bool ignoreFree) {
	int iFreeParticles = 0;
	for (unsigned int i = 0; i < particles.size(); i++){
		SwarmParticle * p = particles[i];
		p->updatePosition(timeStep);
		if(p->isFree()){
			++iFreeParticles;
		}
	}
	if(!ignoreFree && iFreeParticles < particles.size() * 0.1){
		ofLog(OF_LOG_WARNING, "WARNING - PARTICLE LEAK - FREE ALL!");
		freeAllParticles();
	}
	nFree = iFreeParticles;
}


void SwarmParticleSystem::draw(bool circle) {
	int n = particles.size();
	if(!circle){
		glPointSize(1);
		glBegin(GL_POINTS);
	}
	for (int i = 0; i < n; i++){
		if(circle){
			particles[i]->draw();
		}else{
			particles[i]->drawVertex();
		}
	}
	if(!circle){
		glEnd();
	}
}

void SwarmParticleSystem::freeAllParticles(){
	int n = particles.size();
	for (int i = 0; i < n; i++){
		SwarmParticle * p = particles[i];
		p->setFree(true);
	}
}
