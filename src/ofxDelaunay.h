/*
 *  ofxDelaunay.h
 *
 *  Created by Pat Long on 29/10/09.
 *  Copyright 2009 Tangible Interaction. All rights reserved.
 *
 *  Some parts based on demo code by Gilles Dumoulin. 
 *  Source: http://local.wasp.uwa.edu.au/~pbourke/papers/triangulate/cpp.zip
 *
 *	Cleaned up by Lukasz Karluk, 19/05/2010
 *
 *  Refactored for 0071 + ofMesh by James George on 21/06/2012
 *
 *  Modifications by Scott Kildall, 2014
 *
 */

#pragma once

#include "ofMain.h"
#include "Delaunay.h"
#include "ofxSTLPrimitive.h"

class ofxDelaunay {
    
public:
    ofxDelaunay();
    ~ofxDelaunay();
    
	void reset();
	
	int addPoint( const ofPoint& point );
	int addPoint( float x, float y, float z);
	int addPoints( vector<ofPoint>& points );
	
	int  triangulate();
	void draw(Boolean bTriangleDraw);
	
    ofMesh triangleMesh;
    
    //-- SK: added
    void setSize(float _s) { s= _s; }
    void stlOutput(ofxSTLExporter &stlExporter);
    
private:
    
    //-- SK: added
    void drawVertexPoints(XYZ &vertex1, XYZ &vertex2);
    void drawTriangles();
    void drawSTLMeshes();
    void generateSTLMeshes();
    double radians (double d);
    double degrees (double r);

    void initSTLMesh(ofxSTLBoxPrimitive *stlMesh, XYZ &vertex1, XYZ &vertex2);
    void initRotation(ofxSTLBoxPrimitive *stlMesh, XYZ &vertex1, XYZ &vertex2);
    
    float s;
    unsigned long numTriangles;
    ofxSTLBoxPrimitive *STLMeshes;
    unsigned long numSTLMeshes;
    //-- done SK added
    
    vector<ITRIANGLE> triangles;
    vector<XYZ> vertices;
	
};


