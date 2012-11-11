#include "SwarmGui.h"

ofxPanel SwarmGui::gui = ofxPanel();
ofxFloatSlider SwarmGui::timeStep = ofxFloatSlider();
ofxFloatSlider SwarmGui::maxNeighborhood = ofxFloatSlider();

void SwarmGui::setup(string filename){
	gui.setup("GUI",filename,10,85);
	//general
	gui.add(timeStep.setup("timestep", 0.2, 0, 1));
	gui.add(maxNeighborhood.setup("max neighborhood", 30, 0, 100));
}

SwarmSettings * SwarmGui::createSwarmSettings(string name,string shortname){
	ofxPanel * newPanel = new ofxPanel();
	SwarmSettings * settings = new SwarmSettings();
	newPanel->setup(name);
	newPanel->add(settings->dampingForce.setup(shortname+" damping force", 0.05, 0, 1));
	newPanel->add(settings->maxSpeed.setup(shortname+" max speed", 4, 0, 10));
	//flock
	newPanel->add(settings->separatorForce.setup(shortname+" separatorForce", 1.82, 0, 4));
	newPanel->add(settings->alignForce.setup(shortname+" alignForce", 0.1, 0, 4));
	newPanel->add(settings->cohesionForce.setup(shortname+" cohesionForce", 0.59, 0, 3));
	newPanel->add(
			settings->desiredSeparation.setup(shortname+" desired separation", 25, 1, 100));
	newPanel->add(settings->alignRange.setup(shortname+" align range", 20, 1, 100));
	newPanel->add(settings->cohesionRange.setup(shortname+" cohesion range", 50, 1, 100));
	newPanel->add(settings->maxForce.setup(shortname+" max Force", 0.17, 0, 1));
	//particle
	newPanel->add(settings->originForce.setup(shortname+" origin force", 0.005, 0, 0.1));
	newPanel->add(settings->zForce.setup(shortname+" Z force", 0.005, 0, 0.1));
//	newPanel->add(settings->slowForce.setup(shortname+" slow force", 0.9, 0.5, 1));
//	newPanel->add(settings->slowReset.setup(shortname+" slow reset", 1.02, 1, 1.2));
	gui.add(newPanel);

	return settings;
}
