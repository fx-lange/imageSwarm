// openFrameworks version of Daniel Shiffman's Flow Field (<http://www.shiffman.net>)

#ifndef FLOWFIELD_H_
#define FLOWFIELD_H_

#include "ofMain.h"

class FlowField {
protected:
	// A flow field is a two dimensional array of ofVec3f
	vector<vector<ofVec3f> > field;
	int cols, rows; // Columns and Rows
	int resolution; // How large is each "cell" of the flow field

public:

	bool bSetup;

	FlowField() {
		bSetup = false;
	}

	virtual ~FlowField() {

	}

	void setup(int width, int height, int r);

	void init();

	// Draw every vector
	void display();

	// Renders a vector object 'v' as an arrow and a location 'x,y'
	void drawVector(ofVec3f v, float x, float y, float scale);

	ofVec3f & lookup(ofPoint lookup) {
		int x = constrain(lookup.x / resolution, 0, cols - 1);
		int y = constrain(lookup.y / resolution, 0, rows - 1);
		return field[x][y];
	}

	inline int constrain(float val, int min, int max) {
		val = val > max ? max : val;
		val = val < min ? min : val;
		return val;
	}
};

#endif /* FLOWFIELD_H_ */
