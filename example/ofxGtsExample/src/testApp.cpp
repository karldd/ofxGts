#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	zoom = 0;	
	view_rotx = 0;
    view_roty = 0;
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	

	// Reverse Difference fails
//	surface1.setup("surfaces/cube5");
//	surface2.setup("surfaces/cutter");

//	surface1.setup("surfaces/sponge");
//	surface2.setup("surfaces/cube8");
	
	surface1.setup("surfaces/a8.gts");
	surface2.setup("surfaces/cube");
	
	// TODO: Test the surface.addFace() function with some data
	
	
	op = 0;
	operations[0] = "None";
	operations[1] = "Union";	
	operations[2] = "Intersection";	
	operations[3] = "Difference";	
	operations[4] = "Reverse Difference";	
	
	ofBackground(150, 150, 150);
	
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	
	ofNoFill();

	glPushMatrix();
	glTranslatef(ofGetWidth()/2, ofGetHeight()/2, zoom);
	ofScale(100, 100, 100);
	
	// Orbit
	glRotatef(view_rotx, 1.0f, 0.0f, 0.0f);
	glRotatef(view_roty, 0.0f, 1.0f, 0.0f);
	
	ofSetColor(255, 150, 150);
	surface1.drawLines();
	ofSetColor(150, 255, 150);
	surface2.drawLines();
	
	// TODO: Fix the normals when drawing triangles
	//setupForNoTexturing();
	//resultSurface.drawTriangles();
	
	ofSetColor(50, 50, 50);
	resultSurface.drawLines();
	
	glPopMatrix();
	
	
	ofSetColor(0, 0, 0);
	ofDrawBitmapString("Use the 1,2,3,4 keys to switch between boolean operations", 10, 20);
	ofDrawBitmapString("Current Operation: " + operations[op], 10, 40);
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
		case '1':
			surface1.getUnion(surface2, resultSurface);
			op = 1;
			break;
		case '2':
			surface1.getIntersection(surface2, resultSurface);	
			op = 2;
			break;
		case '3':
			surface1.getDifference(surface2, resultSurface);
			op = 3;
			break;
		case '4':
			surface1.getReverseDifference(surface2, resultSurface);
			op = 4;
			break;
			
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	rotate(x, y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	prev_x = x;
    prev_y = y;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


void testApp::resetRotation() {
	view_rotx = 0.0f;
	view_roty = 1.0f;
}

void testApp::rotate(int x, int y) {
	
	float xDiff = x - prev_x;
	float yDiff = y - prev_y;
	float thetaY = 360.0f * (xDiff * 0.005f);
	float thetaX = 360.0f * (yDiff * 0.005f);
	
	prev_x = x;
	prev_y = y;
	
	view_rotx += thetaX;
	view_roty += thetaY;
	
}

void testApp::setupForNoTexturing(){
	
	//    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
	//glDisable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
	
	//    glDisable(GL_BLEND);
	//    glPolygonMode(GL_FRONT, GL_FILL );
	//    glEnable(GL_CULL_FACE);
	//    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
	
    glColor3f(0.5, 0.5, 0.5);
    GLfloat on[]  = {1.0};
    GLfloat off[] = {0.0};
    glLightModelfv( GL_LIGHT_MODEL_TWO_SIDE,on);
	
	
	GLfloat materialAmbient[]  = { 1.0, 1.0, 1.0, 1.0};
	GLfloat materialDiffuse[]  = { 1.0, 1.0, 1.0, 1.0};
	GLfloat materialSpecular[]  = { 1.0, 1.0, 1.0, 1.0};	
    GLfloat shininess[] = {1.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,  materialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,  materialDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	
    GLfloat lightPosition[] = { 0.0, 500.0, 5000.0, 0.0 };
    GLfloat lightDiffuse[]  = { 1.00, 0.99, 0.98, 1.0};
    GLfloat lightSpecular[] = { 0.10, 0.10, 0.10, 1.0};
    GLfloat lightAmbient[]  = { 0.45, 0.43, 0.44, 1.0};
    glLightfv(GL_LIGHT0,GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,  lightDiffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0,GL_AMBIENT,  lightAmbient);
	
}
