/*
 * SlowParticleSystem.cpp
 *
 *  Created on: Nov 2, 2012
 *      Author: felix
 */

#include "SlowParticleSystem.h"

void SlowParticleSystem::addSlow(float targetX, float targetY, float targetZ, float radius){
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
				SlowParticle * curParticle = (SlowParticle*)curBin[i];
				if ( curParticle->ignoresForces()) {
					continue;
				}
				xd = curParticle->x - targetX;
				yd = curParticle->y - targetY;
				zd = curParticle->z - targetZ;
				length = xd * xd + yd * yd + zd * zd;
				if (length > 0 && length < maxrsq * 2) {
#ifdef DRAW_FORCES
					glVertex2f(targetX, targetY);
					glVertex2f(curParticle.x, curParticle.y);
#endif
				}
				if (length > 0 && length < maxrsq) {
					curParticle->slowMe();
				}
			}
		}
	}
}
