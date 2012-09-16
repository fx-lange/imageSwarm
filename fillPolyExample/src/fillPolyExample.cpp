#include "fillPolyExample.h"


//--------------------------------------------------------------
void fillPolyTest::setup(){
	stepSize = 3;
	test = "spta.de oF71";

	ds.loadText("verdana.ttf", 100, test,stepSize);

}

//--------------------------------------------------------------
void fillPolyTest::update(){
	ds.findPointsToText(stepSize);
}

//--------------------------------------------------------------
void fillPolyTest::draw(){
	ofBackground(55);
	ofSetColor(255,255,255);
	ofPushMatrix();
	ofTranslate(10,200);
//	for(int i=0;i<(int)points.size();++i){
//		ofEllipse(points[i],2,2);
//		if(i%2==0 && i+1 <(int)points.size()){
//			ofLine(points[i],points[i+1]);
//		}
//	}


	ofSetColor(255,0,0);
	for(int i=0;i<(int)ds.size();++i){
		PixelData * pd = ds.getPixel(i);
		ofEllipse(pd->x,pd->y,3,3);
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
