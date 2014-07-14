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
 */

#include "ofxDelaunay.h"

ofxDelaunay::ofxDelaunay() {
    numTriangles = 0;
    numSTLMeshes = 0;
    STLMeshes = NULL;
    s = 6;
}

ofxDelaunay::~ofxDelaunay() {
    if( STLMeshes )
        delete [] STLMeshes;
}

void ofxDelaunay::reset(){
    vertices.clear();
    triangles.clear();
	triangleMesh.clear();
    
}

void ofxDelaunay::stlOutput(ofxSTLExporter &stlExporter) {
    for( unsigned long i = 0; i < numSTLMeshes; i++ ) {
        (STLMeshes+i)->save(stlExporter);
    }
}


int ofxDelaunay::addPoint( const ofPoint& point ){
	return addPoint( point.x, point.y, point.z );
}

int ofxDelaunay::addPoint( float x, float y, float z ){
    XYZ v;
    v.x = x;
    v.y = y;
    v.z = z;
    vertices.push_back(v);
	return vertices.size();
}

int ofxDelaunay::addPoints( vector<ofPoint>& points ){
	for(int i = 0; i < points.size(); i++){
        addPoint( points[i] );
    }
    return vertices.size();
}

int ofxDelaunay::triangulate(){
    
    if(vertices.size() < 3){
        return NULL;
    }
    
    int nv = vertices.size();
    //add 3 emptly slots, required by the Triangulate call
    vertices.push_back(XYZ());
    vertices.push_back(XYZ());
    vertices.push_back(XYZ());
    
    //allocate space for triangle indices
    triangles.resize(3*nv);
    
	int ntri;
	qsort( &vertices[0], vertices.size()-3, sizeof( XYZ ), XYZCompare );
	Triangulate( nv, &vertices[0], &triangles[0], ntri );
	numTriangles = ntri;
    
	// copy triangle data to ofxDelaunayTriangle.
	triangleMesh.clear();
    
    //copy vertices
	for (int i = 0; i < nv; i++){
        triangleMesh.addVertex(ofVec3f(vertices[i].x,vertices[i].y,vertices[i].z));
    }
    
    //copy triagles
	for(int i = 0; i < ntri; i++){
		triangleMesh.addIndex(triangles[ i ].p1);
		triangleMesh.addIndex(triangles[ i ].p2);
		triangleMesh.addIndex(triangles[ i ].p3);
	}
	
    //erase the last three triangles
    vertices.erase(vertices.end()-1);
    vertices.erase(vertices.end()-1);
    vertices.erase(vertices.end()-1);
    
    //-- based on number of triangles, we generate STL meshes
    generateSTLMeshes();
    
	return ntri;
}

void ofxDelaunay::draw(Boolean bTriangleDraw){
    if( bTriangleDraw ) {
        ofSetColor(255);
        drawSTLMeshes();
    }
	else if (ofGetStyle().bFill){
	    triangleMesh.draw();
    }
    else{
        // testing: these should look the same
    	//triangleMesh.drawWireframe();
        drawTriangles();
    }
}

// each triangle edge is one vertex — XYZ is simlilar to ofVec3f
//-- numTriangles is teh array of valid triangles
void ofxDelaunay::drawTriangles() {
    for( unsigned long i = 0; i < numTriangles; i++ ) {
        XYZ v1 = vertices[triangles[i].p1];
        XYZ v2 = vertices[triangles[i].p2];
        XYZ v3 = vertices[triangles[i].p3];
        drawVertexPoints(v1,v2);
        drawVertexPoints(v2,v3);
        drawVertexPoints(v3,v1);
    }
}


void ofxDelaunay::generateSTLMeshes() {
    numSTLMeshes = numTriangles*3;
    STLMeshes = new ofxSTLBoxPrimitive[numSTLMeshes];
    
    unsigned long tIndex = 0;
    for( unsigned long i = 0; i < numSTLMeshes; i+=3 ) {
        XYZ v1 = vertices[triangles[tIndex].p1];
        XYZ v2 = vertices[triangles[tIndex].p2];
        XYZ v3 = vertices[triangles[tIndex].p3];
        
        initSTLMesh( (STLMeshes+i), v1, v2 );
        initSTLMesh( (STLMeshes+i+1), v2, v3 );
        initSTLMesh( (STLMeshes+i+2), v3, v1 );
        
        tIndex++;
    }
}

void ofxDelaunay::initSTLMesh(ofxSTLBoxPrimitive *stlMesh, XYZ &vertex1, XYZ &vertex2) {
    
    // calculate distance between nodes = length of pipe
    float xd = abs(vertex1.x-vertex2.x);
    float yd = abs(vertex1.y-vertex2.y);
    float zd = abs(vertex1.z-vertex2.z);
    float dist = sqrt(xd*xd + yd*yd + zd*zd);
    
    stlMesh->getMeshPtr()->clearVertices();
    ((ofxSTLBoxPrimitive *)stlMesh)->set( dist, s, s );
    
    float px = vertex2.x + (vertex1.x-vertex2.x)/2;
    float py = vertex2.y + (vertex1.y-vertex2.y)/2;
    float pz = vertex2.z + (vertex1.z-vertex2.z)/2;
    
    initRotation( stlMesh, vertex1, vertex2);
    ((ofxSTLBoxPrimitive *)stlMesh)->setPosition( px, py, pz );
    stlMesh->setUseVbo(true);
}

double ofxDelaunay::radians (double d) {
    return d * M_PI / 180;
}

double ofxDelaunay::degrees (double r) {
    return r * 180/ M_PI;
}


void ofxDelaunay::drawSTLMeshes() {
    for( unsigned long i = 0; i < numSTLMeshes; i++ ) {
        (STLMeshes+i)->drawWireframe();
    }
}

void ofxDelaunay::initRotation(ofxSTLBoxPrimitive *stlMesh, XYZ &vertex1, XYZ &vertex2) {
    float xd = abs(vertex1.x-vertex2.x);
    float yd = abs(vertex1.y-vertex2.y);
    float zd = abs(vertex1.z-vertex2.z);
    
    float zyHyp = sqrt( zd*zd + yd*yd );
    float zRot = atan2( zyHyp, (float)(vertex1.x-vertex2.x) );
    
    float xRot = atan2( (float)(vertex1.z-vertex2.z), (float)(vertex1.y-vertex2.y) );
    
    xRot = degrees(xRot);
    zRot = degrees(zRot);
    
    ofQuaternion xQRot;
    xQRot.makeRotate(xRot, 1, 0, 0);
    
    ofQuaternion zQRot;
    zQRot.makeRotate(zRot, 0, 0, 1);
    
    
    ofMesh *m = stlMesh->getMeshPtr();
    
    // Apply combined rotations
    for( int i = 0; i < m->getNumVertices(); i++ )
        m->setVertex(i, (m->getVertex(i) * zQRot * xQRot ) );
}

void ofxDelaunay::drawVertexPoints(XYZ &vertex1, XYZ &vertex2) {
    ofLine(vertex1.x, vertex1.y, vertex1.z, vertex2.x, vertex2.y, vertex2.z);
}