#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

#include "ofxGts.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
		
		ofxGtsSurface surface1, surface2, resultSurface;
		int op;
		string operations[5];
	
		/////////////////////////////////////////
		// VIEW
		/////////////////////////////////////////
		float view_rotx, view_roty; 
		float zoom;
		int prev_x, prev_y;
	
		void resetRotation();
		void rotate(int x, int y);
		
		void setupForNoTexturing();
	
};

#endif
