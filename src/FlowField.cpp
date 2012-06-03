#include "FlowField.h"

void FlowField::setup(int width, int height, int _depth	, int r) {
	resolution = r;
	cols = width / resolution;
	rows = height / resolution;
	depth = _depth / resolution;
	field.resize(cols);
	for (int i = 0; i < cols; ++i) {
		field[i].resize(rows);
		for(int j=0; j < rows;++j){
			field[i][j].resize(depth);
		}
	}

	bSetup = true;

	init();
}

void FlowField::init() {
	if (!bSetup) {
		ofLog(OF_LOG_WARNING, "call setup() first");
		return;
	}

	// Reseed noise so we get a new flow field every time
	ofSeedRandom();//TODO not working for noise()!
	float seed = ofRandom(10000);
	float xoff = 0;
	for (int i = 0; i < cols; ++i) {
		float yoff = 0;
		for (int j = 0; j < rows; ++j) {
			float zoff = 0;
			for(int k = 0; k < depth; ++k){
				// Use perlin noise to get an angle between 0 and 2 PI
				float theta = ofMap(ofSignedNoise(seed,xoff, yoff,0), 0, 1, 0, TWO_PI);//TODO ofSignedNoise
				float alpha = ofMap(ofSignedNoise(seed,xoff, 0, zoff), 0 ,1 ,0, TWO_PI);
				// Polar to cartesian coordinate transformation to get x and y components of the vector
				field[i][j][k].set(cos(theta), sin(theta), sin(alpha));
				zoff += 0.1;
			}
			yoff += 0.1;
		}
		xoff += 0.1;
	}
}

// Draw every vector
void FlowField::display() {
	if (!bSetup) {
		ofLog(OF_LOG_WARNING, "call setup() first");
		return;
	}

	for (int i = 0; i < cols; ++i) {
		for (int j = 0; j < rows; ++j) {
			for( int k = 0; k < depth; ++k){
				drawVector(field[i][j][k], i * resolution, j * resolution,
						 k * resolution ,resolution - 2);
			}
		}
	}

}

// Renders a vector object 'v' as an arrow and a location 'x,y'
void FlowField::drawVector(ofVec3f v, float x, float y, float z, float scale) {
	ofPushMatrix();
	float arrowsize = 4;
	// Translate to location to render vector
	ofTranslate(x, y,z);
	ofSetColor(100,100,100,50);
	// Call vector heading function to get direction (note that pointing up is a heading of 0) and rotateZ
	float rotateZ = atan2(v.y, v.x);
	float rotateY = atan2(v.z, v.x);
	ofRotate(ofRadToDeg(rotateZ), 0, 0, 1);
	ofRotate(ofRadToDeg(rotateY), 0, 1, 0);
	// Calculate length of vector & scale it to be bigger or smaller if necessary

	float len = v.length() * scale;

	// Draw three lines to make an arrow (draw pointing up since we've rotate to the proper direction)
	ofLine(0, 0, 0, len, 0 , 0);
//	ofLine(len, 0, len - arrowsize, +arrowsize / 2);
//	ofLine(len, 0, len - arrowsize, -arrowsize / 2);
	ofPopMatrix();
}
