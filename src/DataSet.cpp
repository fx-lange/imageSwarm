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

int DataSet::loadText(string font,int size,string text,int stepSize){
	ofTrueTypeFont ttf;
	ttf.loadFont(font,size,true,true,true);
	paths = ttf.getStringAsPoints(text);
	boundingBox = ttf.getStringBoundingBox(text,0,0);

	return findPointsToText(stepSize);
}

int DataSet::findPointsToText(int stepSize){
	vector<vector <ofPoint> > lines;
	for(int y=-boundingBox.height;y<boundingBox.height;y+=stepSize){
		float lineY = y;
		vector<ofPoint> points;
		findSortedIntersectionsY(paths,points,lineY,stepSize);
		lines.push_back(points);
	}

//	add inside points
	pixels.clear();
	for(int i=0;i<(int)lines.size();++i){// for each line ..
		vector<ofPoint> & points = lines[i];
		for(int j=0;j<(int)points.size()-1;j+=2){// for each even point
			// ... insert points between the even and next one!
			float diff = points[j+1].x - points[j].x;
			int steps = ceil(diff / (float)stepSize);
			float tmpStepSize = diff / (float)steps;

			for(int k=1;k<steps;++k){
				PixelData * p = new PixelData();
				p->x = (float)k*tmpStepSize+points[j].x;
				p->y = points[j].y;
				p->c = ofColor(255,255,255);
				pixels.push_back(p);
			}
		}
	}

//	add outline points
	for(int pi=0;pi<(int)paths.size() ;++pi){//for each path...
			vector<ofPolyline> & lines = paths[pi].getOutline();

			for (int li = 0; li < (int)lines.size(); ++li) {//for each outline...
				vector<ofPoint> & line = lines[li].getVertices();
				addOutline(line,stepSize);
			}
	}

	cout << pixels.size() << endl;
	return pixels.size();
}

void DataSet::findSortedIntersectionsY(vector<ofTTFCharacter> & paths,vector<ofPoint> & dest,float lineY,int stepSize){

	for(int pi=0;pi<(int)paths.size() ;++pi){//for each path...
		vector<ofPolyline> & lines = paths[pi].getOutline();

		for (int li = 0; li < (int)lines.size(); ++li) {//for each outline...
			vector<ofPoint> & line = lines[li].getVertices();
			ofPoint * lp = &line[line.size()-1];
			ofPoint * ap;

			for(int i=0;i<(int)line.size();++i){//for each neighbor pair...
				ap = &line[i];

				if( (ap->y< lineY && lp->y>= lineY)	|| (lp->y< lineY && ap->y>=lineY)){//check for intersection!
					float intersectionX = ap->x +  (lineY-ap->y)/(lp->y-ap->y) *(lp->x-ap->x);
					dest.push_back(ofVec3f(intersectionX, lineY));
				}
				lp = ap;
			}

		}
	}

	//sort intersections
	sort(dest.begin(),dest.end(),DataSet::xCoordCompare);

}

void DataSet::addOutline(vector<ofPoint> & outline, float maxDistance){
	float tmpStepSize = maxDistance;
	int size = outline.size();
	int idx = -1;
	ofPoint point = outline[size-1];
	while(idx<size-1){
		while(idx<size-1){
			ofVec3f & next = outline[idx+1];

			float distance = point.distance(next);
			if(distance > tmpStepSize){
				point = (next-point).getNormalized()*tmpStepSize + point;
				PixelData * p = new PixelData();
				p->x = point.x;
				p->y = point.y;
				p->c = ofColor(255,255,255);
				pixels.push_back(p);
				tmpStepSize = maxDistance;
				break;
			}else{
				point = next;
				tmpStepSize -= distance;
				idx++;
			}
		}
	}

}

void DataSet::pixelsToParticles(SwarmParticleSystem * ps, bool onlyActive) {
	for (int i = 0; i < size(); ++i) {
		SwarmParticle * swarmParticle = ps->getNextUnused(onlyActive);
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

void DataSet::translateOrigins(float transX, float transY, float transZ,bool bCentered) {
	if(bCentered){
		transX -= boundingBox.width / 2;
	}
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

