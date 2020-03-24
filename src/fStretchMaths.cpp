
// fStretch - a deformation plug-in for Maya
// Copyright (C) 2015  Matthieu Fiorilli

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Details of the license at: http://www.gnu.org/licenses/gpl.html
// More info at www.cgaddict.com


#include <maya/MItMeshVertex.h>

// ################################################################################################
// ############################################ MATH PROCS ########################################
// ################################################################################################
//
// Those maths are used for our custom wrap deformer
//

// DIVISION BY ZERO CHECK
float noZero(double value){
    if (value == 0){ return 0.000001; }
	else { return value; }
}

// VECTOR NORMALIZATION
MVector normalizeVector(MVector vec){
	return (vec / noZero(sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z)));
}

// DOT PRODUCT
float dotP(MVector A, MVector B)
{
    return A.x*B.x+A.y*B.y+A.z*B.z;
}

// CROSS PRODUCT
MVector crossP(MVector A, MVector B)
{
	MVector result;
	result.x = A.y * B.z - A.z * B.y;
	result.y = A.z * B.x - A.x * B.z;
	result.z = A.x * B.y - A.y * B.x;

    return result;
}

// CLOSEST POINT ON EDGE
MVector closestPointOnEdge(MVector edge1, MVector edge2, MVector point){
    MVector u = normalizeVector(edge2 - edge1);
    MVector v = point - edge1;
	return ((dotP(u,v) * u) + edge1);
}

// CLOSEST POINT ON PLANE and its DISTANCE
MPoint closestPointOnPlane(MVector faceNormal, MVector facePoint, MVector point)
{

    MVector temp;
    float distance = dotP(faceNormal, point-facePoint);

    temp = point - distance*faceNormal;
	
	MPoint result;
	result.x = temp.x;
	result.y = temp.y;
	result.z = temp.z;
	result.w = distance;
    return result;
}

// DISTANCE 2 POINTS
float distance3D(MVector a, MVector b)
{
    return sqrt(((b.x - a.x)*(b.x - a.x)) + ((b.y - a.y)*(b.y - a.y)) + ((b.z - a.z)*(b.z - a.z)));
}

// INTERSECTION OF 2 LINES
MVector intersectTwoLines(MVector A1, MVector A2, MVector B1, MVector B2){
    float nA = dotP(crossP(B2-B1,A1-B1),crossP(A2-A1,B2-B1));
    float nB = dotP(crossP(A2-A1,A1-B1),crossP(A2-A1,B2-B1));
    float d = dotP(crossP(A2-A1,B2-B1),crossP(A2-A1,B2-B1));
    MVector line1 = A1 + (nA/noZero(d))*(A2-A1);
    //if we need the intersection of the 2nd line -> line2 = B1 + (nB/d)*(B2-B1);
    return line1;
}

// NORMAL GIVEN 3 VTX

MVector getNormal(MPoint A, MPoint B, MPoint C)
{
    MVector v1;
	v1.x = B.x - A.x;
	v1.y = B.y - A.y;
	v1.z = B.z - A.z;

	MVector v2;
	v2.x = C.x - A.x;
	v2.y = C.y - A.y;
	v2.z = C.z - A.z;

    MVector normal;
	normal.x = (v1.y * v2.z) - (v1.z * v2.y);
	normal.y = -((v2.z * v1.x) - (v2.x * v1.z));
	normal.z = (v1.x * v2.y) - (v1.y * v2.x);
    
    return normalizeVector(normal);
}

// ROUNDOFF A VALUE

// int round(float value) {
// 	return int(value + 0.5);
// }

// proc to find the cos of the angle of two vectors
// 1 = 0 degree
// 0 = 90 degree
// -1 = 180 degree

// here is a version that works with the face normal.
// 1 = 0 degree positive
// 0 = 180 degree
// -1 = 0 degree negative

float vectorAngle(MVector A, MVector B, MVector normal){
	// if weird result.. try normalizeVector(A+B)
	return ((dotP( normalizeVector(A), normalizeVector(B) )+1)/2) * dotP(normalizeVector((A+B)),normal);
}


// procs to retrieve the value needed to control direction UV based on the painted weights
/*
in   U  V
0    1  0
.25  1  .5
.5   1  1
.75 .5  1
1    0  1
*/

float convertPaintedU(float input){
	float result = (input*2);
	if (result > 1) { result = 1; }
	return result;
}
float convertPaintedV(float input){
	float result = (1-input)*2;
	if (result > 1) { result = 1; }
	return result;
}