
#include "ofMain.h"
#include "gts.h"

class ofxGtsSurface {
	
public:

	
	ofxGtsSurface();
	~ofxGtsSurface();
	
	/* Setup an empty surface */
	void setup();
	/* Setup a surface by loading a GTS file from the data folder*/
	void setup(string filename);
	/* Setup a surface with a GtsSurface */
	void setup(GtsSurface *s);
	
	/* Draw the surface */
	void drawTriangles();
	void drawLines();
	
	/* Add a face to the surface */
	void addFace(ofPoint &p1, ofPoint &p2, ofPoint &p3);
	
	/* Boolean operations between this surface and the source surface
	 * The new surface is loaded into the result surface */
	void getUnion(ofxGtsSurface &source, ofxGtsSurface &result);
	void getIntersection(ofxGtsSurface &source, ofxGtsSurface &result);
	void getDifference(ofxGtsSurface &source, ofxGtsSurface &result);
	void getReverseDifference(ofxGtsSurface &source, ofxGtsSurface &result);
	
	/* Print stats about this surface */
	void printStats();
	
	GtsSurface *surface;
	
protected:
	
	/* If the surface has been loaded */
	bool loaded;
	
	enum DrawType{
		LINES,
		TRIANGLES
	};
	
	void draw(DrawType type);
	
	/* The type of boolean operation */
	enum BooleanOperation {
		BOOLEAN_INTERSECTION, 
		BOOLEAN_UNION, 
		BOOLEAN_DIFFERENCE, 
		BOOLEAN_REVERSE_DIFFERENCE
	};
	
	/* Prepare for a boolean operation, checking the validity of the surface */
	bool prepareBoolean(ofxGtsSurface &source);
	/* Create the boolean surface */
	void createBoolean(ofxGtsSurface &source, ofxGtsSurface &result, BooleanOperation operation);
	
	bool boolPerformed;
	GtsSurfaceInter * si;
	GNode *tree1, *tree2;
	gboolean is_open1, is_open2;

	
};
