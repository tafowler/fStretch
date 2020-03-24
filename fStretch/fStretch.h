
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


#ifndef fStretch_h
#define fStretch_h

#ifndef _DEBUG
#ifdef _OPENMP
    #include <omp.h> // This is for OpenMP multi-threading. OpenMP DOES NOT WORK IN DEBUG
#endif
#endif

#include <iostream>
#include <maya/MPxDeformerNode.h>
#include <maya/MItGeometry.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFnMeshData.h>
#include <maya/MItMeshVertex.h>
#include <maya/MThreadUtils.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <stddef.h>
#include <Iphlpapi.h>
#include <Assert.h>
#include <maya/MItMeshEdge.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MRampAttribute.h>
#include <ctime>

using namespace std;
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <tchar.h>
#include <stdio.h>

// Include the procedures of our other files

/////////////////////////////////// fWrap procs ///////////////////////////////////

MVector getWrapValues(MPoint A, MPoint B, MPoint C, MVector faceNormal, MVector point);
MVector getWrapResult(MPoint A, MPoint B, MPoint C, MVector faceNormal, float U, float V, float N);
MIntArray orderFacesByNeighboors_decomposeIntoTriArray(MIntArray facesArray, MIntArray vtxPerFaceList, int oneTriPerFace);
void drawTri(MIntArray orderedFaces, int i);
MIntArray trisPerRadius(int currentRadius, int itemCount, int smoothRadius, float itemsPerRadius, float centralFactor);

/////////////////////////////////// fWrap Math Procs ///////////////////////////////////

float noZero(double value);
MVector normalizeVector(MVector vec);
float dotP(MVector A, MVector B);
MVector crossP(MVector A, MVector B);
MVector closestPointOnEdge(MVector edge1, MVector edge2, MVector point);
MPoint closestPointOnPlane(MVector faceNormal, MVector facePoint, MVector point);
float distance3D(MVector a, MVector b);
MVector intersectTwoLines(MVector A1, MVector A2, MVector B1, MVector B2);
MVector getNormal(MPoint A, MPoint B, MPoint C);
int round(float value);
float vectorAngle(MVector A, MVector B, MVector normal);
float convertPaintedU(float input);
float convertPaintedV(float input);


/////////////////////////////////// Init / Uninit ///////////////////////////////////

MStatus uninitializePlugin( MObject obj);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////// MACROS ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define MCheckStatus(status,message) \
	if( MStatus::kSuccess != status ) { \
		MGlobal::displayError(message "\n"); \
		return status; \
	}

class fStretch : public MPxDeformerNode
{

public:
	fStretch();
	virtual ~fStretch();

	static void* creator();
	static MStatus initialize();

	// Deformation function

	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock);


public:
	// Local node attributes

	static MTypeId id;
	static MObject displayWarnings;
	static MObject displayComputeTime;
	static MObject scaleFactor;

	static MObject stretchGrowShrink;
	static MObject squashGrowShrink;
	static MObject tensionPostSmooth;

	static MObject bendInGrowShrink;
	static MObject bendOutGrowShrink;
	static MObject anglePostSmooth;

	static MObject restMesh;
	static MObject squashMesh;
	static MObject stretchMesh;
	static MObject bendInMesh;
	static MObject bendOutMesh;

	static MObject UsquashMesh;
	static MObject VsquashMesh;
	static MObject UstretchMesh;
	static MObject VstretchMesh;
	static MObject UbendInMesh;
	static MObject VbendInMesh;
	static MObject UbendOutMesh;
	static MObject VbendOutMesh;

	static MObject tensionEnvelope;
	static MObject tensionUDirection;
	static MObject tensionVDirection;
	static MObject squashMultiplier;
	static MObject stretchMultiplier;
	static MObject tensionSmooth;
	static MObject tensionSmoothAttribute;
	static MObject paintTensionSmooth;
	static MObject drawColorPerVertex;

	static MObject squashRelax;
	static MObject stretchRelax;
	static MObject squashRange;
	static MObject stretchRange;
	static MObject tensionColorContrast;
	static MObject angleColorContrast;

	static MObject angleEnvelope;
	static MObject angleSmooth;
	static MObject angleSmoothAttribute;
	static MObject paintAngleSmooth;
	static MObject bendIn;
	static MObject bendOut;
	static MObject angleUDirection;
	static MObject angleVDirection;
	static MObject bendInFalloff;
	static MObject bendOutFalloff;
	static MObject bendInRange;
	static MObject bendOutRange;
	static MObject bendInRelax;
	static MObject bendOutRelax;

	static MObject wrap;
	static MObject wrapSmoothRadius;
	static MObject wrapItemsPerRadius;
	static MObject wrapCentralFactor;
	static MObject wrapOneTriPerFace;

	static MObject stretchFalloff;
	static MObject squashFalloff;
	static MObject tensionColorRamp;
	static MObject angleColorRamp;
	static MObject colorDisplay;
	static MObject forceColorRefresh;

	static MObject tensionDirectionAttribute;
	static MObject paintTensionDirection;
	static MObject angleDirectionAttribute;
	static MObject paintAngleDirection;

	static MObject stretchGrowShrinkAttribute;
	static MObject paintStretchGrowShrink;
	static MObject squashGrowShrinkAttribute;
	static MObject paintSquashGrowShrink;
	static MObject bendInGrowShrinkAttribute;
	static MObject paintBendInGrowShrink;
	static MObject bendOutGrowShrinkAttribute;
	static MObject paintBendOutGrowShrink;

	static MObject paintTensionRange;
	static MObject tensionRangeAttribute;
	static MObject paintAngleRange;
	static MObject angleRangeAttribute;

	static MObject paintStretchRelax;
	static MObject stretchRelaxAttribute;
	static MObject paintSquashRelax;
	static MObject squashRelaxAttribute;
	static MObject paintBendInRelax;
	static MObject bendInRelaxAttribute;
	static MObject paintBendOutRelax;
	static MObject bendOutRelaxAttribute;

	static MObject counterSharpAreas;
	static MObject tensionFalloff;
	static MObject angleFalloff;

	static MObject UseAllAvailableProcessors;
	static MObject NumberOfProcessorsToUse;

private:
	// Create the MATRIX for the CONNECTED VERTEX (for stretch/squash calculations)
	typedef int* matrixPtr;
	matrixPtr* connectedVertsMatrix;

	// Create the MATRIX for the wrap deformer. Each element represent a vertex of the mesh and it is filled with the nearest triangles and their relation to the vertex.
	typedef float* matrixFloatPtr;
	matrixFloatPtr* vertexTrisMatrix;

	// Create the MATRIX to store the UV direction
	matrixFloatPtr* UVDirectionMatrix;

	// tension Array
	float* stretchArray;

	// tensionSmooth Array
	float* tensionSmoothArray; 

	// Track down if the MATRIX IS CREATED
	int isMatrixCreated;

	// Var to know if it's the first time it computes
	int firstComputeDone;

	// Last number of vertex/edges/faces
	int lastNumberOfVertex;
	int lastNumberOfEdges;
	int lastNumberOfPoly;

	// Last wrap values
	bool lastWrapData;
	int lastWrapSmoothRadius;
	float lastWrapItemsPerRadius;
	float lastWrapCentralFactor;
	bool lastWrapOneTriPerFace;

	// Last acceleration values
	bool lastUseAllAvailableProcessors;
	int lastNumberOfProcessorsToUse;

	// Last direction values
	float lastTensionUDirection;
	float lastTensionVDirection;
	float lastAngleUDirection;
	float lastAngleVDirection;

	// UVDirection warning time before warning again
	time_t UVDirectionTime;

	// Var to always turn off color display when we checked off the color per vertex display
	int colorPerVertexDisplay;

	// Time vars
	time_t start_time;
	time_t cur_time;

	// Create meshes vars globally so we can cache them
	MPointArray vertsUSquash;
	MPointArray vertsVSquash;
	MPointArray vertsUStretch;
	MPointArray vertsVStretch;
	MPointArray vertsUBendIn;
	MPointArray vertsVBendIn;
	MPointArray vertsUBendOut;
	MPointArray vertsVBendOut;

	MPointArray vertsRest;
	MPointArray vertsSquash;
	MPointArray vertsStretch;
	MPointArray vertsBendIn;
	MPointArray vertsBendOut;

	MFloatVectorArray meshNormalsRest;

	// Track down if the arrays/matrices have been created so we delete them accordingly

	int stretchArrayExists;
	int tensionSmoothArrayExists;
	int vertexTrisMatrixExists;
	int connectedVertsMatrixExists;
	int UVDirectionMatrixExists;
};

#endif