#include "testApp.h"

void schwarmApp::loadDataSets() {

	//TODO forschleife?!
	int loaded = 0;
	if( whichScene == 1){
		loaded += f1.loadImage("scene1/1.png", 3, true);
		f1.translateOrigins(sceneOffset, sceneOffset);
		loaded += f2.loadImage("scene1/2.png", 3, true);
		f2.translateOrigins(sceneOffset, sceneOffset);
		loaded += f3.loadImage("scene1/3.png", 3, true);
		f3.translateOrigins(sceneOffset, sceneOffset);
		loaded += f4.loadImage("scene1/madewith.png", 3, true);
		f4.translateOrigins(sceneOffset, sceneOffset);
		loaded += f5.loadImage("scene1/open.png", 3, true);
		f5.translateOrigins(sceneOffset, sceneOffset);
		loaded += f5a.loadImage("scene1/source.png", 3, true);
		f5a.translateOrigins(sceneOffset, sceneOffset);
		loaded += f5b.loadImage("scene1/Frameworks.png", 3, true);
		f5b.translateOrigins(sceneOffset, sceneOffset);
	}
	ofLog(OF_LOG_NOTICE, ofToString(loaded) + " particle loaded");
}

void schwarmApp::addParticles(int amount, bool active) {
	float padding = 15;
	float maxVelocity = .05;

	for (int i = 0; i < amount; i++) {
		float x = ofRandom(padding+sceneOffset, scene.width - padding + sceneOffset);
		float y = ofRandom(padding+sceneOffset, scene.height - padding + sceneOffset);
		float z = ofRandom(-sceneDepth + padding, sceneDepth - padding);
		float xv = ofRandom(-maxVelocity, maxVelocity);
		float yv = ofRandom(-maxVelocity, maxVelocity);
		SwarmParticle * particle = new SwarmParticle(x, y, xv, yv);

		if(whichScene == 2){
			particle->origin.z = -sceneDepth/2;
		}
		particle->z = z;
		particle->setActive(active);
		ps.add(particle);
	}
}

//--------------------------------------------------------------
void schwarmApp::setup() {
	ofSetFrameRate(30);

//fast settings
	whichScene = 1;
	sceneCounter = 1; //change to start at a later point in the scene

	scene.width = 1024;
	scene.height = 768;
//	scene.width = 450;
//	scene.height = 200;
	sceneDepth = 150;
	sceneOffset = 100;

	dataMoveOffsetX = 25;
	dataMoveOffsetY = 10;

	useOld = false;

	//shader
	bUseShader = true;
	if (bUseShader) {
		dofShader.load("shaders/dof");
		fbo.allocate(scene.width, scene.height);
	}

	//cam
	cam.setDistance(500);

	//content
	loadDataSets();

	//swarm particle system
	int binPower = 3;
	ps.setup(scene.width + sceneOffset * 2, scene.height + sceneOffset * 2,
			binPower);

	kParticles = 5;

	if(whichScene == 1){
		kParticles = 3;
		addParticles(kParticles * 1000, false);
	}
	addParticles(1 * 1000, true);

	setupGui();
}

void schwarmApp::setupGui() {
	filename = "settings/settings_";
	filename += ofToString(whichScene) + ".xml";
	gui.setup("gui",filename);
	gui.setSize(300, gui.getHeight());
	//general
	gui.add(timeStep.setup("timestep", 0.2, 0, 1));
	gui.add(maxNeighborhood.setup("max neighborhood", 30, 0, 100));

	freePanel.setup("FREE");
	freePanel.add(free.dampingForce.setup("F damping force", 0.05, 0, 1));
	freePanel.add(free.maxSpeed.setup("F max speed", 4, 0, 10));
	//flock
	freePanel.add(free.separatorForce.setup("F separatorForce", 1.82, 0, 4));
	freePanel.add(free.alignForce.setup("F alignForce", 0.1, 0, 4));
	freePanel.add(free.cohesionForce.setup("F cohesionForce", 0.59, 0, 3));
	freePanel.add(
			free.desiredSeparation.setup("F desired separation", 25, 1, 100));
	freePanel.add(free.alignRange.setup("F align range", 20, 1, 100));
	freePanel.add(free.cohesionRange.setup("F cohesion range", 50, 1, 100));
	freePanel.add(free.maxForce.setup("F max Force", 0.17, 0, 1));
	//particle
	freePanel.add(free.originForce.setup("F origin force", 0.005, 0, 0.1));
	freePanel.add(free.zForce.setup("F Z force", 0.005, 0, 0.1));
	gui.add(&freePanel);

	originPanel.setup("ORIGIN");
	originPanel.add(origin.dampingForce.setup("damping force", 0.05, 0, 1));
	originPanel.add(origin.maxSpeed.setup("max speed", 4, 0, 10));
	//flock
	originPanel.add(origin.separatorForce.setup("separatorForce", 1.82, 0, 4));
	originPanel.add(origin.alignForce.setup("alignForce", 0.1, 0, 2));
	originPanel.add(origin.cohesionForce.setup("cohesionForce", 0.59, 0, 3));
	originPanel.add(
	origin.desiredSeparation.setup("desired separation", 25, 1, 100));
	originPanel.add(origin.alignRange.setup("align range", 20, 1, 100));
	originPanel.add(origin.cohesionRange.setup("cohesion range", 50, 1, 100));
	originPanel.add(origin.maxForce.setup("max Force", 0.17, 0, 1));
	//particle
	originPanel.add(origin.originForce.setup("origin force", 0.005, 0, 0.1));
	originPanel.add(origin.zForce.setup("Z force", 0.005, 0, 0.1));
	gui.add(&originPanel);

	//shader
	shaderPanel.setup("Shader");
	shaderPanel.add(focusDistance.setup("focus distance", 300, 0, 2000));
	shaderPanel.add(aperture.setup("aperture", 0.01, 0, 0.5));
	shaderPanel.add(pointBrightness.setup("point brightness", 1, 0.7, 1));
	shaderPanel.add(maxPointSize.setup("maxPointSize", 10, 1, 50));
	gui.add(&shaderPanel);

	//extras
	extras.setup("Extras");
	extras.add(translateSpeed.setup("movement pixel/f",0.5,0,5));
	extras.add(boerseMinX.setup("minX",0,0,scene.width/2));
	extras.add(boerseMinY.setup("minY",0,0,scene.width/2));
	extras.add(boerseMaxX.setup("maxX",scene.width,0,scene.width));
	extras.add(boerseMaxY.setup("maxY",scene.height,0,scene.height));
	extras.add(videoAlpha.setup("video alpha",255,0,255));
	gui.add(&extras);

	gui.loadFromFile(filename);
	bHide = true;
}

//--------------------------------------------------------------
void schwarmApp::update() {

	//particle system
	ps.setupForces();
	ps.setTimeStep(timeStep);

	for (unsigned int i = 0; i < ps.size(); i++) {
		SwarmParticle & cur = ps[i];
		if (!cur.isActive())
			continue;

		float originForce, dampingForce;
		if (cur.state == PARTICLE_ORIGIN) {
			cur.maxSpeed = origin.maxSpeed;
			cur.cohesionForce = origin.cohesionForce;
			cur.alignForce = origin.alignForce;
			cur.separatorForce = origin.separatorForce;
			cur.maxForce = origin.maxForce;
			cur.zForce = -origin.zForce;
			originForce = origin.originForce;
			dampingForce = origin.dampingForce;
			cur.alignRange = origin.alignRange;
			cur.cohesionRange = origin.cohesionRange;
			cur.desiredSeparation = origin.desiredSeparation;
		} else {
			cur.maxSpeed = free.maxSpeed;
			cur.cohesionForce = free.cohesionForce;
			cur.alignForce = free.alignForce;
			cur.separatorForce = free.separatorForce;
			cur.maxForce = free.maxForce;
			cur.zForce = -free.zForce;
			cur.alignRange = free.alignRange;
			cur.cohesionRange = free.cohesionRange;
			cur.desiredSeparation = free.desiredSeparation;
			originForce = free.originForce;
			dampingForce = free.dampingForce;
		}

//		if (cur.state != PARTICLE_ORIGIN) { //TODO possible to ignore flock if origindistance is small

			vector<SwarmParticle *> neighbors = ps.getNeighbors(cur,
					maxNeighborhood);

			if (useOld) {
				cur.oldflock(neighbors);
			} else {
				cur.flock(neighbors);
			}

//		}

		cur.borders(50, 50, scene.width - 50 + sceneOffset * 2,
				scene.height - 50 + sceneOffset * 2, -sceneDepth, sceneDepth);

		cur.addOriginForce(-originForce);
		cur.addDampingForce(dampingForce);
	}

	if (ofGetMousePressed() && bHide && !cam.getMouseInputEnabled())
		ps.addRepulsionForce(mouseX, mouseY, 0, 100, 10);

	ps.update();
}
//--------------------------------------------------------------
void schwarmApp::draw() {
	ofEnableAlphaBlending();
	ofBackground(0, 0, 0);
	ofSetColor(255, 255, 255);

	if (bUseShader) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_ALPHA_TEST);

		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);

		fbo.begin();
		ofClear(0, 0, 0, 0);
		dofShader.begin();
		dofShader.setUniform1f("focusDistance", focusDistance);
		dofShader.setUniform1f("aperture", aperture);
		dofShader.setUniform1f("pointBrightness", pointBrightness);
		dofShader.setUniform1f("maxPointSize", maxPointSize);
	} else {
		cam.begin();
		ofPushMatrix();
		ofTranslate(-scene.width / 2, -scene.height / 2);
	}


	// ---------DRAW SCENE----------

	ofPushMatrix();
	ofTranslate(-sceneOffset, -sceneOffset);
	ps.draw(false);
	ofPopMatrix();

	// -----------------------------

	if (bUseShader) {
		dofShader.end();
		fbo.end();

		ofPushMatrix();
		fbo.draw(0, 0);
		ofSetColor(255, 255, 255, videoAlpha);

		if (whichScene == 2)
			player.draw(0, 0);

		ofPopMatrix();
	} else {
		ofPopMatrix();
		ofPushMatrix();
//			ofPushMatrix();
//			ofPushStyle();
//	//		ofSetRectMode(OF_RECTMODE_CENTER);
//			ofTranslate
//			ofRect(boerseMinX,boerseMinY,boerseMaxX,boerseMaxY);
//			ofPopStyle();
//			ofPopMatrix();
		ofScale(scene.width / 10, scene.height / 10, sceneDepth / 5);
		ofNoFill();
		ofBox(10);
		ofPopMatrix();
		cam.end();
		cam.draw();
	}

	ofPopMatrix();

	if (!bHide)
		gui.draw();
	ofDrawBitmapString(ofToString(ofGetFrameRate()),10,10);
}

void schwarmApp::exit() {
}


void schwarmApp::playScene1() {
	switch (sceneCounter) {
	case 1:
		useDataSet(f1);
		break;
	case 2:
		freeDataSet(f1);
//		useDataSet(f2);
		f2.reuseDataSet(&ps,&f1,40);

		break;
	case 3:
		freeDataSet(f2);
		useDataSet(f3);
		break;
	case 4:
		freeDataSet(f3);
		useDataSet(f4);
		break;
	case 5:
		useDataSet(f5);
		useDataSet(f5a);
		break;
	case 6:
		freeDataSet(f5a);
		useDataSet(f5b); //BALL
		break;
	case 7:
		freeDataSet(f5);
		freeDataSet(f5b);
		freeDataSet(f4);
		sceneCounter = -1;
		break;

	}
	ps.printNumbers();
	sceneCounter++;
}

void schwarmApp::keyPressed(int key) {
	switch (key) {
	case 'o':
		useOld = !useOld;
		break;
	case 'c':
		if (cam.getMouseInputEnabled())
			cam.disableMouseInput();
		else
			cam.enableMouseInput();
		break;
	case 'd':
		bUseShader = !bUseShader;
		break;
	case 'g':
		bHide = !bHide;
		break;
	}

	if (key == ' ') {
		if (whichScene == 1)
			playScene1();
	}

}

//--------------------------------------------------------------
void schwarmApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void schwarmApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void schwarmApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void schwarmApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void schwarmApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void schwarmApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void schwarmApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void schwarmApp::dragEvent(ofDragInfo dragInfo) {

}