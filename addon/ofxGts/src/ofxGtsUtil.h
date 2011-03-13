
#include "gts.h"

static void pick_first_face (GtsFace * f, GtsFace ** first) {
	if (*first == NULL)
		*first = f;
}

static void prepend_triangle_bbox (GtsTriangle * t, GSList ** bboxes) {
	*bboxes = g_slist_prepend (*bboxes, 
							   gts_bbox_triangle (gts_bbox_class (), t));
}

ofPoint calculateNormal(GtsPoint &p1, GtsPoint &p2, GtsPoint &p3){
	
	// Vector of each side
//	ofxVec3f v1 = p3 - p2;
//	ofxVec3f v2 = p1 - p2;
//	ofxVec3f normal = v1.crossed(v2);
//	normal.normalize();
	
	float v1x = p3.x - p2.x;
	float v1y = p3.y - p2.y;
	float v1z = p3.z - p2.z;
	
	float v2x = p1.x - p2.x;
	float v2y = p1.y - p2.y;
	float v2z = p1.z - p2.z;
	
	float crossX = v1y*v2z - v1z*v2y;
	float crossY = v1z*v2x - v1x*v2z;
	float crossZ = v1x*v2y - v1x*v2x;
	
	float length = (float)sqrt(crossX*crossX + crossY*crossY + crossZ*crossZ);
	if( length > 0 ) {
		return ofPoint(crossX/length, crossY/length, crossZ/length);
	} 
	return NULL;
}