#include "fillPolyExample.h"

bool xCoordCompare(const ofPoint&lp, const ofPoint&rp){
	return lp.x<=rp.x;
}

void findSortedIntersectionsY(vector<ofTTFCharacter> & paths,vector<ofPoint> & dest,float lineY,int stepSize){

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
	sort(dest.begin(),dest.end(),xCoordCompare);

}

void addOutline(vector<ofPoint> & outline, vector<ofPoint> & dst,float maxDistance){
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
				dst.push_back(point);
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

//--------------------------------------------------------------
void fillPolyTest::setup(){
	stepSize = 3;
	test = "spta.de oF71";
	font.loadFont("verdana.ttf", 100, true, true,true);
	paths = font.getStringAsPoints(test);

}

//--------------------------------------------------------------
void fillPolyTest::update(){
	ofRectangle bb = font.getStringBoundingBox(test,0,0);
//	float lineY = -1.f* bb.height*(1.f-(float)mouseY/(float)ofGetHeight());
//	float lineY = -1.f*((float)mouseY-200.f);
	lines.clear();
	for(int y=-bb.height;y<bb.height;y+=stepSize){
		float lineY = y;
		vector<ofPoint> points;
		findSortedIntersectionsY(paths,points,lineY,stepSize);
		lines.push_back(points);
	}

//	add inside points
	formPoints.clear();
	for(int i=0;i<(int)lines.size();++i){// for each line ..
		vector<ofPoint> & points = lines[i];
		for(int j=0;j<(int)points.size()-1;j+=2){// for each even point
			// ... insert points between the even and next one!
			float diff = points[j+1].x - points[j].x;
			int steps = ceil(diff / (float)stepSize);
			float tmpStepSize = diff / (float)steps;

			for(int k=1;k<steps;++k){
				formPoints.push_back(ofPoint((float)k*tmpStepSize+points[j].x,points[j].y));
			}
		}
	}

//	add outline points
	for(int pi=0;pi<(int)paths.size() ;++pi){//for each path...
			vector<ofPolyline> & lines = paths[pi].getOutline();

			for (int li = 0; li < (int)lines.size(); ++li) {//for each outline...
				vector<ofPoint> & line = lines[li].getVertices();
				addOutline(line,formPoints,stepSize);
			}
	}

	cout << formPoints.size() << endl;
}

//--------------------------------------------------------------
void fillPolyTest::draw(){
	ofBackground(55);
	ofSetColor(255,255,255);
	ofPushMatrix();
	ofTranslate(10,200);
	font.drawString(test,0,0);
	ofSetColor(255,0,0);
//	for(int i=0;i<(int)points.size();++i){
//		ofEllipse(points[i],2,2);
//		if(i%2==0 && i+1 <(int)points.size()){
//			ofLine(points[i],points[i+1]);
//		}
//	}

	ofSetColor(255,0,0);
	for(int i=0;i<(int)formPoints.size();++i){
		ofEllipse(formPoints[i],3,3);
	}
	ofPopMatrix();

	ofDrawBitmapString(ofToString(ofGetFrameRate()),10,10);
}

//--------------------------------------------------------------
void fillPolyTest::keyPressed(int key){
	switch(key){
	case '+':
		stepSize++;
		break;
	case '-':
		stepSize--;
		if(stepSize<1)
			stepSize = 1;
		break;
	}
}

//--------------------------------------------------------------
void fillPolyTest::keyReleased(int key){

}

//--------------------------------------------------------------
void fillPolyTest::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void fillPolyTest::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void fillPolyTest::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void fillPolyTest::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void fillPolyTest::windowResized(int w, int h){

}

//--------------------------------------------------------------
void fillPolyTest::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void fillPolyTest::dragEvent(ofDragInfo dragInfo){

}
//ZUM SPRENGEN VON BUCHSTABEN
//void addOutline(vector<ofPoint> & outline, vector<ofPoint> & dst,float maxDistance){
//	float tmpStepSize = maxDistance;
//	int size = outline.size();
//	int idx = 0;
//	ofPoint point = outline[idx];
//	while(idx<size-1){
//		while(idx<size-1){
//			ofVec3f & next = outline[idx+1];
//
//			float distance = point.distance(next);
//			if(distance > tmpStepSize){
//				point = (next-point).getNormalized()*maxDistance + point;
//				dst.push_back(point);
//				tmpStepSize = maxDistance;
//				break;
//			}else{
//				point = next;
//				tmpStepSize -= distance;
//				idx++;
//			}
//		}
//	}
//
//}
