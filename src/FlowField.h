// openFrameworks version of Daniel Shiffman's Flow Field (<http://www.shiffman.net>)

#ifndef FLOWFIELD_H_
#define FLOWFIELD_H_

#include "ofMain.h"

class FlowField {
protected:
	// A flow field is a two dimensional array of ofVec3f
	vector< vector<ofVec3f> > field;
	int cols, rows; // Columns and Rows
	int resolution; // How large is each "cell" of the flow field


public:
	FlowField();
	virtual ~FlowField();


	bool bSetup;

	FlowField() {
	  bSetup = false;
	}

	void setup(int width, int height, int r) {
		resolution = r;
		cols = width/resolution;
		rows = height/resolution;
		field.resize(cols);
		for(unsigned int i=0;i<cols;++i){
			field[i].resize(rows);
		}

		init();
	}

	void init(){
		if(!bSetup){
			ofLog(OF_LOG_WARNING,"call setup() first");
			return;
		}

		// Reseed noise so we get a new flow field every time
		ofSeedRandom();
		float xoff = 0;
		for (int i = 0; i < cols; i++) {
		  float yoff = 0;
		  for (int j = 0; j < rows; j++) {
			// Use perlin noise to get an angle between 0 and 2 PI
			float theta = map(ofNoise(xoff,yoff),0,1,0,TWO_PI);
			// Polar to cartesian coordinate transformation to get x and y components of the vector
			field[i][j].set(cos(theta),sin(theta));
			yoff += 0.1;
		  }
		  xoff += 0.1;
		}
	}

	  // Draw every vector
	  void display() {
		if(!bSetup){
			ofLog(OF_LOG_WARNING,"call setup() first");
			return;
		}

	    for (int i = 0; i < cols; i++) {
	      for (int j = 0; j < rows; j++) {
	        drawVector(field[i][j],i*resolution,j*resolution,resolution-2);
	      }
	    }

	  }

	  // Renders a vector object 'v' as an arrow and a location 'x,y'
	  void drawVector(ofVec3f v, float x, float y, float scale) {
	    ofPushMatrix();
	    float arrowsize = 4;
	    // Translate to location to render vector
	    ofTranslate(x,y);
	    ofSetColor(100);
	    // Call vector heading function to get direction (note that pointing up is a heading of 0) and rotate
	    float rotate = atan2(v.y, v.x);
	    ofRotate(ofRadToDeg(rotate), 0, 0, 1);
	    // Calculate length of vector & scale it to be bigger or smaller if necessary

	    float len = v.length() * scale;

	    // Draw three lines to make an arrow (draw pointing up since we've rotate to the proper direction)
	    ofLine(0,0,len,0);
	    ofLine(len,0,len-arrowsize,+arrowsize/2);
	    ofLine(len,0,len-arrowsize,-arrowsize/2);
	    ofPopMatrix();
	  }

	  ofVec3f & lookup(ofPoint lookup) {
	    int x = constrain(lookup.x/resolution,0,cols-1);
	    int y = constrain(lookup.y/resolution,0,rows-1);
	    return field[x][y];
	  }


};

int constrain(float val,int min, int max){
	val = val > max ? max : val;
	val = val < min ? min : val;
	return val;
}









#endif /* FLOWFIELD_H_ */
