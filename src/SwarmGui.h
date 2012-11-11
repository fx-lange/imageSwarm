#ifndef SWARMGUI_H_
#define SWARMGUI_H_

#include "ofxGui.h"

struct SwarmSettings {
	ofxFloatSlider originForce;
	ofxFloatSlider dampingForce;
	ofxFloatSlider maxSpeed;

	ofxFloatSlider separatorForce;
	ofxFloatSlider desiredSeparation;
	ofxFloatSlider alignForce;
	ofxFloatSlider alignRange;
	ofxFloatSlider cohesionForce;
	ofxFloatSlider cohesionRange;
	ofxFloatSlider maxForce;
	ofxFloatSlider zForce;
	ofxFloatSlider slowForce;
	ofxFloatSlider slowReset;
};

struct SwarmGui {
	static ofxPanel gui;

	static ofxFloatSlider timeStep;
	static ofxFloatSlider maxNeighborhood;

	static SwarmSettings * createSwarmSettings(string name,string shortname);
	static void setup(string filename);
};

#endif /* SWARMGUI_H_ */
