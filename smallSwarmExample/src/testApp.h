#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ParticleSystem.h"
#include "DataSet.h"
#include "ofxGui.h"
#include "ofxVideoRecorder.h"

class SwarmSettings {
public:
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
};

class schwarmApp: public ofBaseApp {

public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void exit();

	void loadDataSets();
	void playScene1();

	void addParticles(int amount, bool free);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void useDataSet(DataSet & ds,float extraDataScale = 0,bool bNotFree = true) {
		ds.pixelsToParticles(&ps,bNotFree);
	}

	void freeDataSet(DataSet & ds,int freeModulo = 1) {
		ds.freeParticles(&ps,freeModulo);
	}
	/* GUI */
	bool bHide;
	string filename;
	//panels
	ofxPanel gui;
	ofxPanel originPanel;
	ofxPanel freePanel;
	ofxPanel shaderPanel;
	//particle
	SwarmSettings origin, free;
	ofxFloatSlider timeStep;
	ofxFloatSlider maxNeighborhood;

	//shader
	ofxFloatSlider focusDistance;
	ofxFloatSlider aperture;
	ofxFloatSlider pointBrightness;
	ofxIntSlider maxPointSize;

	/* Swarm */
	SwarmParticleSystem ps;

	/* World */
	ofRectangle scene;
	float sceneDepth;
	float sceneOffset; //difference between particle world and scene

	/* Content */
	int animationCounter;

	//Scene 1
	DataSet f1, f0a, f0b, f2, f3, f4, f5, f5a,f5b;

	/* 3D */
	ofEasyCam cam;

	/* Shader */
	bool bUseShader;
	ofShader dofShader;
	ofFbo fbo;
};

