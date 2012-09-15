#include "testApp.h"

bool compareFiles(ofFile lf,ofFile rf){
	return lf.getFileName() <= rf.getFileName();
}

void schwarmApp::loadDataSets() {
	ofDirectory DIR("scene1");
	DIR.listDir();
	vector<ofFile> files = DIR.getFiles();
	ofSort(files,compareFiles);
	int loaded = 0;
	for(int i=0;i<(int)files.size();++i){
		DataSet ds;
		loaded += ds.loadImage(files[i].getAbsolutePath(),3,true);
		ds.translateOrigins(sceneOffset, sceneOffset);
		datasets.push_back(ds);
	}
	ofLog(OF_LOG_NOTICE,ofToString(loaded) + " points loaded");
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

		particle->z = z;
		particle->setActive(active);
		ps.add(particle);
	}
}

//--------------------------------------------------------------
void schwarmApp::setup() {
	ofSetFrameRate(30);
	ofSetLogLevel(OF_LOG_VERBOSE);

//settings
	animationCounter = 1; //change to start at a later point in the scene

	scene.width = 475;
	scene.height = 340;
	sceneDepth = 150;
	sceneOffset = 100;

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

	//setup swarm particle system
	int binPower = 3;
	ps.setup(scene.width + sceneOffset * 2, scene.height + sceneOffset * 2,
			binPower);

	//add 1k hidden particles
	addParticles(1 * 1000, false);
	//add 1k visible particles
	addParticles(1 * 1000, true);

	setupGui();
}

void schwarmApp::setupGui() {
	filename = "settings.xml";
	gui.setup("gui",filename,10,85);
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


		vector<SwarmParticle *> neighbors = ps.getNeighbors(cur, maxNeighborhood);
		cur.flock(neighbors);

		cur.borders(50, 50, scene.width - 50 + sceneOffset * 2,
				scene.height - 50 + sceneOffset * 2, -sceneDepth, sceneDepth);

		cur.addOriginForce(-originForce);
		cur.addDampingForce(dampingForce);
	}

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
		ofScale(2,2,1);
		fbo.draw(0, 0);
		ofPopMatrix();
	} else {
		ofPopMatrix();
		ofPushMatrix();
		ofScale((scene.width+ sceneOffset * 2) / 10, (scene.height+ sceneOffset * 2) / 10, sceneDepth / 5);
		ofNoFill();
		ofBox(10);
		ofPopMatrix();
		cam.end();
		cam.draw();
	}

	ofPopMatrix();

	if (!bHide){
		gui.draw();
	}

	string msg = ofToString(ofGetFrameRate());
	msg += "\r\n\r\nControls: \r\n";
	msg += "SPACE - next animation step\r\n";
	msg += "t - toggle 3d view\r\n";
	msg += "g - toggle gui\r\n";
	ofDrawBitmapString(msg,10,10);
}

void schwarmApp::exit() {
}


void schwarmApp::playScene1() {
	//hard coded animation

	switch (animationCounter) {
	case 1:
		datasets[0].pixelsToParticles(&ps); //1
		break;
	case 2:
		datasets[0].freeParticles(&ps);
		datasets[1].pixelsToParticles(&ps); //2

		break;
	case 3:
		datasets[1].freeParticles(&ps);
		datasets[2].pixelsToParticles(&ps); //3
		break;
	case 4:
		datasets[2].freeParticles(&ps);
		datasets[3].reuseDataSet(&ps,&datasets[2],50); //made with
		break;
	case 5:
		datasets[4].pixelsToParticles(&ps); //open
		datasets[5].pixelsToParticles(&ps); //source
		break;
	case 6:
		datasets[5].freeParticles(&ps);
		datasets[6].reuseDataSet(&ps,&datasets[5],40); //frameworks
		break;
	case 7:
		//free all
		datasets[3].freeParticles(&ps);
		datasets[4].freeParticles(&ps);
		datasets[6].freeParticles(&ps);
		animationCounter = 0;
		break;

	}
	ps.printNumbers();
	animationCounter++;
}

void schwarmApp::keyPressed(int key) {
	switch (key) {
	case 't':
		bUseShader = !bUseShader;
		break;
	case 'g':
		bHide = !bHide;
		break;
	case ' ':
		playScene1();
		break;
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
