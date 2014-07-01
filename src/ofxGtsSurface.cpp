
#include "ofxGtsSurface.h"
#include "ofxGtsUtil.h"

ofxGtsSurface::ofxGtsSurface() {
	loaded = false;
	boolPerformed = false;
}

ofxGtsSurface::~ofxGtsSurface() {
	if(loaded) {
		/* destroy surfaces and intersection */
		gts_object_destroy(GTS_OBJECT(surface));		
		
		if(boolPerformed) {
			gts_object_destroy(GTS_OBJECT(si));
			
			/* destroy bounding box trees (including bounding boxes) */
			gts_bb_tree_destroy(tree1, TRUE);
			gts_bb_tree_destroy(tree2, TRUE);  
		}
	}
		
}

void ofxGtsSurface::setup() {
	surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
							  GTS_FACE_CLASS(gts_nface_class()), 
							  GTS_EDGE_CLASS(gts_nedge_class()), 
							  GTS_VERTEX_CLASS(gts_nvertex_class()));
	loaded = true;
}

void ofxGtsSurface::setup(string filename) {

	FILE * fptr;
	GtsFile * fp;
	
	string filePath = ofToDataPath(filename);
	
	/* open first file */
	if ((fptr = fopen (filePath.c_str(), "rt")) == NULL) {		
		ofLog(OF_LOG_ERROR, "Cannot open file: " + filePath);
		return;
	}
	/* reads in first surface file */
	surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
							  GTS_FACE_CLASS(gts_nface_class()), 
							  GTS_EDGE_CLASS(gts_nedge_class()), 
							  GTS_VERTEX_CLASS(gts_nvertex_class()));
	fp = gts_file_new(fptr);
	if (gts_surface_read (surface, fp)) {
		ofLog(OF_LOG_ERROR, filePath + " is not a valid GTS surface file");
		loaded = false;
		gts_object_destroy(GTS_OBJECT(surface));
	} else {
		ofLog(OF_LOG_NOTICE, "Gts surface file read: " + filePath);
		loaded = true;
	}
	gts_file_destroy (fp);
	fclose (fptr);	
}

void ofxGtsSurface::setup(GtsSurface *s) {
	surface = s;
	loaded = true;
}

void ofxGtsSurface::drawTriangles() {
	draw(TRIANGLES);
}

void ofxGtsSurface::drawLines() {
	draw(LINES);
}

// TODO: Is there a better way to draw stuff
void ofxGtsSurface::draw(DrawType type) {
	
	if(!loaded) {
		//ofLog(OF_LOG_NOTICE, "Gts surface not loaded");
		return;
	}
	
	GtsFace * first = NULL;
	// TODO: Do we really need to do this just to get the first face?
	gts_surface_foreach_face (surface, (GtsFunc) pick_first_face, &first);
	
	if (first) {
		GtsSurfaceTraverse * t = gts_surface_traverse_new (surface, first);
		GtsFace * f;
		guint level;

		glBegin((type == TRIANGLES) ? GL_TRIANGLE_STRIP : GL_LINES);
		while ((f = gts_surface_traverse_next (t, &level))) {

			// Edge 1
			GtsPoint p1 = f->triangle.e1->segment.v1->p;
			GtsPoint p2 = f->triangle.e1->segment.v2->p;
			// Edge 2			
			GtsPoint p3 = f->triangle.e2->segment.v1->p;
			GtsPoint p4 = f->triangle.e2->segment.v2->p;
			// Edge 3
			GtsPoint p5 = f->triangle.e3->segment.v1->p;
			GtsPoint p6 = f->triangle.e3->segment.v2->p;
			

			// TODO: Figure out what is going on with the normals
			if(type==TRIANGLES) {
				ofPoint normal = calculateNormal(p1, p2, p3);
				glNormal3f(normal.x, normal.y, normal.z);
			}
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);			
			glVertex3f(p3.x, p3.y, p3.z);
			
			
			if(type==TRIANGLES) {
				ofPoint normal = calculateNormal(p4, p5, p6);
				glNormal3f(normal.x, normal.y, normal.z);
			}
			glVertex3f(p4.x, p4.y, p4.z);			
			glVertex3f(p5.x, p5.y, p5.z);
			glVertex3f(p6.x, p6.y, p6.z);			
			
			
		}
		glEnd();
		gts_surface_traverse_destroy (t);
	}
}

void ofxGtsSurface::addFace(ofPoint &p1, ofPoint &p2, ofPoint &p3) {
	
	if(!loaded) {
		setup();
	}
	
	GtsVertex *v1 = gts_vertex_new(surface->vertex_class, p1.x, p1.y, p1.z);
	GtsVertex *v2 = gts_vertex_new(surface->vertex_class, p2.x, p2.y, p2.z);
	GtsVertex *v3 = gts_vertex_new(surface->vertex_class, p3.x, p3.y, p3.z);	
	
	GtsEdge *e1 = gts_edge_new(surface->edge_class, v1, v2);
	GtsEdge *e2 = gts_edge_new(surface->edge_class, v2, v3);
	GtsEdge *e3 = gts_edge_new(surface->edge_class, v3, v1);	
	
	gts_surface_add_face(surface, gts_face_new(surface->face_class, e1, e2, e3));
	
}

bool ofxGtsSurface::prepareBoolean(ofxGtsSurface &source) {
	if(!loaded || !source.loaded) {
		ofLog(OF_LOG_NOTICE, "Gts surface not loaded, could not perform boolean operation");
		return false;
	}
	
	if(!gts_surface_is_orientable(surface)) {
		ofLog(OF_LOG_ERROR, "Gts surface is not an orientable manifold, could not perform boolean operation");
		return false;
	}
	
	if(gts_surface_is_self_intersecting(surface)) {
		ofLog(OF_LOG_ERROR, "Gts surface self-intersects, could not perform boolean operation");
		return false;
	}
	
	if(!gts_surface_is_orientable(source.surface)) {
		ofLog(OF_LOG_ERROR, "Gts surface is not an orientable manifold, could not perform boolean operation");
		return false;
	}
	
	if(gts_surface_is_self_intersecting(source.surface)) {
		ofLog(OF_LOG_ERROR, "Gts surface self-intersects, could not perform boolean operation");
		return false;
	}
	
	GSList *bboxes = NULL;
	gts_surface_foreach_face(surface, (GtsFunc) prepend_triangle_bbox, &bboxes);
	/* build bounding box tree for first surface */
	tree1 = gts_bb_tree_new(bboxes);
	/* free list of bboxes */
	g_slist_free (bboxes);
	is_open1 = gts_surface_volume(surface) < 0. ? TRUE : FALSE;
	
	/* build bounding boxes for second surface */
	bboxes = NULL;
	gts_surface_foreach_face(source.surface, (GtsFunc) prepend_triangle_bbox, &bboxes);
	/* build bounding box tree for second surface */
	tree2 = gts_bb_tree_new(bboxes);
	/* free list of bboxes */
	g_slist_free (bboxes);
	is_open2 = gts_surface_volume (source.surface) < 0. ? TRUE : FALSE;
	
	si = gts_surface_inter_new (gts_surface_inter_class (), 
								surface, source.surface, tree1, tree2, is_open1, is_open2);
	
	gboolean closed = TRUE;
	gts_surface_inter_check(si, &closed);
	boolPerformed = true;
	if(!closed) {
		ofLog(OF_LOG_NOTICE, "Gts surface is not closed, could not perform boolean operation");
		return false;
	}

	return true;
}

void ofxGtsSurface::createBoolean(ofxGtsSurface &source, ofxGtsSurface &result, BooleanOperation operation) {

	result.surface = gts_surface_new(GTS_SURFACE_CLASS(gts_surface_class()), 
									 GTS_FACE_CLASS(gts_nface_class()), 
									 GTS_EDGE_CLASS(gts_nedge_class()), 
									 GTS_VERTEX_CLASS(gts_nvertex_class()));
	switch(operation) {
			
		case BOOLEAN_INTERSECTION:
			gts_surface_inter_boolean(si, result.surface, GTS_1_IN_2);
			gts_surface_inter_boolean(si, result.surface, GTS_2_IN_1);
			result.loaded = true;
			break;
			
		case BOOLEAN_UNION:
			gts_surface_inter_boolean(si, result.surface, GTS_1_OUT_2);
			gts_surface_inter_boolean(si, result.surface, GTS_2_OUT_1);
			result.loaded = true;
			break;
			
		case BOOLEAN_DIFFERENCE:
			gts_surface_inter_boolean(si, result.surface, GTS_1_OUT_2);
			gts_surface_inter_boolean(si, result.surface, GTS_2_IN_1);
			gts_surface_foreach_face(si->s2, (GtsFunc)gts_triangle_revert, NULL);
			gts_surface_foreach_face(source.surface, (GtsFunc)gts_triangle_revert, NULL);
			result.loaded = true;
			break;
			
		case BOOLEAN_REVERSE_DIFFERENCE:
			// TODO: Reverse difference can cause crashes, is there a way to catch them?
			gts_surface_inter_boolean(si, result.surface, GTS_2_OUT_1);
			gts_surface_inter_boolean(si, result.surface, GTS_1_IN_2);
			gts_surface_foreach_face(si->s1, (GtsFunc)gts_triangle_revert, NULL);
			gts_surface_foreach_face(surface, (GtsFunc)gts_triangle_revert, NULL);
			result.loaded = true;
			break;
	}
	
}

void ofxGtsSurface::getUnion(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_UNION);
	} 
}

void ofxGtsSurface::getIntersection(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_INTERSECTION);
	} 
}

void ofxGtsSurface::getDifference(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_DIFFERENCE);
	} 					  
}
			  
void ofxGtsSurface::getReverseDifference(ofxGtsSurface &source, ofxGtsSurface &result) {
	if( prepareBoolean(source)) {
		createBoolean(source, result, BOOLEAN_REVERSE_DIFFERENCE);
	} 					  
}

void ofxGtsSurface::printStats() {
	if(loaded) {
		gts_surface_print_stats (surface, stdout);		
	} else {
		ofLog(OF_LOG_NOTICE, "Gts surface not loaded");
	}

}

