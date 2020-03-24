
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


#include "fStretch.h"


MTypeId fStretch::id( 0x00114B40 );
MObject fStretch::displayWarnings;
MObject fStretch::displayComputeTime;
MObject fStretch::scaleFactor;

MObject fStretch::stretchGrowShrink;
MObject fStretch::squashGrowShrink;
MObject fStretch::tensionPostSmooth;

MObject fStretch::bendInGrowShrink;
MObject fStretch::bendOutGrowShrink;
MObject fStretch::anglePostSmooth;

MObject fStretch::restMesh;
MObject fStretch::squashMesh;
MObject fStretch::stretchMesh;
MObject fStretch::bendInMesh;
MObject fStretch::bendOutMesh;

MObject fStretch::UsquashMesh;
MObject fStretch::VsquashMesh;
MObject fStretch::UstretchMesh;
MObject fStretch::VstretchMesh;
MObject fStretch::UbendInMesh;
MObject fStretch::VbendInMesh;
MObject fStretch::UbendOutMesh;
MObject fStretch::VbendOutMesh;

MObject fStretch::tensionEnvelope;
MObject fStretch::tensionUDirection;
MObject fStretch::tensionVDirection;
MObject fStretch::squashMultiplier;
MObject fStretch::stretchMultiplier;

MObject fStretch::tensionSmooth;
MObject fStretch::paintTensionSmooth;
MObject fStretch::tensionSmoothAttribute;
MObject fStretch::drawColorPerVertex;
MObject fStretch::squashRelax;
MObject fStretch::stretchRelax;
MObject fStretch::squashRange;
MObject fStretch::stretchRange;
MObject fStretch::tensionColorContrast;
MObject fStretch::angleColorContrast;

MObject fStretch::angleEnvelope;
MObject fStretch::angleSmooth;
MObject fStretch::paintAngleSmooth;
MObject fStretch::angleSmoothAttribute;
MObject fStretch::bendIn;
MObject fStretch::bendOut;
MObject fStretch::angleUDirection;
MObject fStretch::angleVDirection;
MObject fStretch::bendInFalloff;
MObject fStretch::bendOutFalloff;
MObject fStretch::bendInRange;
MObject fStretch::bendOutRange;
MObject fStretch::bendInRelax;
MObject fStretch::bendOutRelax;

MObject fStretch::wrap;
MObject fStretch::wrapSmoothRadius;
MObject fStretch::wrapItemsPerRadius;
MObject fStretch::wrapCentralFactor;
MObject fStretch::wrapOneTriPerFace;

MObject fStretch::stretchFalloff;
MObject fStretch::squashFalloff;
MObject fStretch::tensionColorRamp;
MObject fStretch::angleColorRamp;
MObject fStretch::colorDisplay;
MObject fStretch::forceColorRefresh;

MObject fStretch::tensionDirectionAttribute;
MObject fStretch::paintTensionDirection;
MObject fStretch::angleDirectionAttribute;
MObject fStretch::paintAngleDirection;

MObject fStretch::stretchGrowShrinkAttribute;
MObject fStretch::paintStretchGrowShrink;
MObject fStretch::squashGrowShrinkAttribute;
MObject fStretch::paintSquashGrowShrink;
MObject fStretch::bendInGrowShrinkAttribute;
MObject fStretch::paintBendInGrowShrink;
MObject fStretch::bendOutGrowShrinkAttribute;
MObject fStretch::paintBendOutGrowShrink;

MObject fStretch::paintTensionRange;
MObject fStretch::tensionRangeAttribute;
MObject fStretch::paintAngleRange;
MObject fStretch::angleRangeAttribute;

MObject fStretch::paintStretchRelax;
MObject fStretch::stretchRelaxAttribute;
MObject fStretch::paintSquashRelax;
MObject fStretch::squashRelaxAttribute;
MObject fStretch::paintBendInRelax;
MObject fStretch::bendInRelaxAttribute;
MObject fStretch::paintBendOutRelax;
MObject fStretch::bendOutRelaxAttribute;

MObject fStretch::counterSharpAreas;
MObject fStretch::tensionFalloff;
MObject fStretch::angleFalloff;

MObject fStretch::UseAllAvailableProcessors;
MObject fStretch::NumberOfProcessorsToUse;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////// COMPUTE ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MStatus fStretch::compute(const MPlug& plug, MDataBlock& data)

{

	bool failed = false;

	// Do this if we are using an OpenMP implementation that is not the same as Maya's.
	// Even if it is the same, it does no harm to make this call.
	MThreadUtils::syncNumOpenMPThreads();

	MStatus status = MStatus::kUnknownParameter;
	if (plug.attribute() != outputGeom) {
		return status;
	}

	unsigned int index = plug.logicalIndex();
	MObject thisNode = this->thisMObject();

	// Get computeTime Attr
	MDataHandle displayComputeTimeData = data.inputValue( displayComputeTime, &status );
	MCheckStatus(status, "fStretch: Cannot get the displayComputeTime attr.");

time(&cur_time);

int clo;
if (displayComputeTimeData.asBool() == true) {
clo = clock();
cout <<"\n\n--------------------------------------------------------------------------------------- \n";
cout <<"----------------------------- fStretch's computation time ----------------------------- \n";
cout <<"--------------------------------------------------------------------------------------- \n\n ";
}

	//############################## Get all VALUES / DATA ##############################

	// Get ENVELOPE value
	MDataHandle envData = data.inputValue( envelope );
	float env = envData.asFloat();

	// Get INPUT value
	MPlug inPlug(thisNode,input);
	inPlug.selectAncestorLogicalIndex(index,input);
if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting the Envelope and the Input values. \n "; clo = clock(); }
	MDataHandle hInput = data.inputValue(inPlug, &status);
	MCheckStatus(status, "ERROR getting input mesh\n");
if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting the input mesh. \n "; clo = clock(); }
	// Get the INPUT GEOMETRY
	MDataHandle inputData = hInput.child(inputGeom);
	if (inputData.type() != MFnData::kMesh) {

		MGlobal::displayError("fStretch: Incorrect input geometry type. The geometry must be polygonal.\n ");
		return MStatus::kFailure;
	}
	// Get the OUTPUT DATA
	MDataHandle outputData = data.outputValue(plug);

	outputData.copy(inputData);
 	if (outputData.type() != MFnData::kMesh) {
		MGlobal::displayError("fStretch: Incorrect output mesh type.\n ");
		return MStatus::kFailure;
	}
	// Get the input groupId - ignored for now...

	MDataHandle hGroup = inputData.child(groupId);
	unsigned int groupId = hGroup.asLong();

	// If envelope is zero, spit out the mesh and stop here
	if (env <= .0001){
		MItGeometry iter(outputData, groupId, false);
		MPointArray verts;
		iter.allPositions(verts);
		return MStatus::kSuccess;
	}

if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting the inputData and the outputData.\n "; clo = clock(); }

	// Get the Paint DATA
	MArrayDataHandle tensionSmoothPaintarrayHandle = data.inputArrayValue(paintTensionSmooth, &status);
	MArrayDataHandle angleSmoothPaintarrayHandle = data.inputArrayValue(paintAngleSmooth, &status);
	MArrayDataHandle paintTensionDirectionarrayHandle = data.inputArrayValue(paintTensionDirection, &status);
	MArrayDataHandle paintAngleDirectionarrayHandle = data.inputArrayValue(paintAngleDirection, &status);

	MArrayDataHandle paintStretchGrowShrinkarrayHandle = data.inputArrayValue(paintStretchGrowShrink, &status);
	MArrayDataHandle paintSquashGrowShrinkarrayHandle = data.inputArrayValue(paintSquashGrowShrink, &status);
	MArrayDataHandle paintBendInGrowShrinkarrayHandle = data.inputArrayValue(paintBendInGrowShrink, &status);
	MArrayDataHandle paintBendOutGrowShrinkarrayHandle = data.inputArrayValue(paintBendOutGrowShrink, &status);

	MArrayDataHandle paintTensionRangearrayHandle = data.inputArrayValue(paintTensionRange, &status);
	MArrayDataHandle paintAngleRangearrayHandle = data.inputArrayValue(paintAngleRange, &status);

	MArrayDataHandle paintStretchRelaxarrayHandle = data.inputArrayValue(paintStretchRelax, &status);
	MArrayDataHandle paintSquashRelaxarrayHandle = data.inputArrayValue(paintSquashRelax, &status);
	MArrayDataHandle paintBendInRelaxarrayHandle = data.inputArrayValue(paintBendInRelax, &status);
	MArrayDataHandle paintBendOutRelaxarrayHandle = data.inputArrayValue(paintBendOutRelax, &status);


if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Creating the Paint handles. \n "; clo = clock(); }

	//############################## Get all MESHES ##############################

	// Get REST mesh
	int isRestMeshClean = data.isClean(restMesh);
	MDataHandle restData = data.inputValue(restMesh, &status);
	if ( !isRestMeshClean || (vertsRest.length() == 0) ){
		if (restData.type() != MFnData::kMesh) { restData.copy(inputData); if (restData.type() != MFnData::kMesh) { MGlobal::displayError("fStretch: Incorrect input mesh type.\n"); return MStatus::kFailure; } }
		MItGeometry iterRest(restData, groupId, false);	iterRest.allPositions(vertsRest);
	}

	// Get SQUASH mesh
	int isSquashMeshClean = data.isClean(squashMesh);
	MDataHandle squashData = data.inputValue(squashMesh, &status);
	if ( !isSquashMeshClean || (vertsSquash.length() == 0) ){
		if (squashData.type() != MFnData::kMesh) { squashData.copy(inputData); if (squashData.type() != MFnData::kMesh) { MGlobal::displayError("fStretch: Incorrect input mesh type.\n"); return MStatus::kFailure; } }
		MItGeometry iterSquash(squashData, groupId, false);	iterSquash.allPositions(vertsSquash);
	}

	// Get STRETCH mesh
	int isStretchMeshClean = data.isClean(stretchMesh);
	MDataHandle stretchData = data.inputValue(stretchMesh, &status);
	if ( !isStretchMeshClean || (vertsStretch.length() == 0) ){
		if (stretchData.type() != MFnData::kMesh) { stretchData.copy(inputData); if (stretchData.type() != MFnData::kMesh) { MGlobal::displayError("fStretch: Incorrect input mesh type.\n"); return MStatus::kFailure; } }
		MItGeometry iterStretch(stretchData, groupId, false); iterStretch.allPositions(vertsStretch);
	}

	// Get BENDIN mesh
	int isBendInMeshClean = data.isClean(bendInMesh);
	MDataHandle bendInMeshData = data.inputValue(bendInMesh, &status);
	if ( !isBendInMeshClean || (vertsBendIn.length() == 0) ){
		if (bendInMeshData.type() != MFnData::kMesh) { bendInMeshData.copy(inputData); if (bendInMeshData.type() != MFnData::kMesh) { MGlobal::displayError("fStretch: Incorrect input mesh type.\n"); return MStatus::kFailure; } }
		MItGeometry iterBendIn(bendInMeshData, groupId, false);	iterBendIn.allPositions(vertsBendIn);
	}
	// Get BENDOUT mesh
	int isBendOutMeshClean = data.isClean(bendOutMesh);
	MDataHandle bendOutMeshData = data.inputValue(bendOutMesh, &status);
	if ( !isBendOutMeshClean || (vertsBendOut.length() == 0) ){
		if (bendOutMeshData.type() != MFnData::kMesh) { bendOutMeshData.copy(inputData); if (bendOutMeshData.type() != MFnData::kMesh) { MGlobal::displayError("fStretch: Incorrect input mesh type.\n"); return MStatus::kFailure; } }
		MItGeometry iterBendOut(bendOutMeshData, groupId, false); iterBendOut.allPositions(vertsBendOut);
	}

if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting The Stretch, Squash, Bend In, Bend Out and Rest meshes. \n "; clo = clock(); }

	if ( !data.isClean(UsquashMesh) || (vertsUSquash.length() == 0) || !isSquashMeshClean ){
		// Get USQUASH mesh
		MDataHandle UsquashData = data.inputValue(UsquashMesh, &status);
		if (UsquashData.type() != MFnData::kMesh) { MItGeometry iterUSquash(squashData, groupId, false); iterUSquash.allPositions(vertsUSquash); }
		else {										MItGeometry iterUSquash(UsquashData, groupId, false); iterUSquash.allPositions(vertsUSquash); }
	}
	if (!data.isClean(VsquashMesh) || (vertsVSquash.length() == 0) || !isSquashMeshClean ){
		// Get VSQUASH mesh
		MDataHandle VsquashData = data.inputValue(VsquashMesh, &status);
		if (VsquashData.type() != MFnData::kMesh) { MItGeometry iterVSquash(squashData, groupId, false); iterVSquash.allPositions(vertsVSquash); }
		else {										MItGeometry iterVSquash(VsquashData, groupId, false); iterVSquash.allPositions(vertsVSquash); }
	}
	if (!data.isClean(UstretchMesh) || (vertsUStretch.length() == 0) || !isStretchMeshClean ){
		// Get USTRETCH mesh
		MDataHandle UstretchData = data.inputValue(UstretchMesh, &status);
		if (UstretchData.type() != MFnData::kMesh) { MItGeometry iterUStretch(stretchData, groupId, false); iterUStretch.allPositions(vertsUStretch); }
		else {										 MItGeometry iterUStretch(UstretchData, groupId, false); iterUStretch.allPositions(vertsUStretch); }
	}
	if (!data.isClean(VstretchMesh) || (vertsVStretch.length() == 0) || !isStretchMeshClean ){
		// Get VSTRETCH mesh
		MDataHandle VstretchData = data.inputValue(VstretchMesh, &status);
		if (VstretchData.type() != MFnData::kMesh) { MItGeometry iterVStretch(stretchData, groupId, false); iterVStretch.allPositions(vertsVStretch); }
		else {										 MItGeometry iterVStretch(VstretchData, groupId, false); iterVStretch.allPositions(vertsVStretch); }
	}
	if (!data.isClean(UbendInMesh) || (vertsUBendIn.length() == 0) || !isBendInMeshClean ){
		// Get UBEND mesh
		MDataHandle UbendInMeshData = data.inputValue(UbendInMesh, &status);
		if (UbendInMeshData.type() != MFnData::kMesh) {	MItGeometry iterUBendIn(bendInMeshData, groupId, false); iterUBendIn.allPositions(vertsUBendIn); }
		else {											MItGeometry iterUBendIn(UbendInMeshData, groupId, false); iterUBendIn.allPositions(vertsUBendIn); }
	}
	if (!data.isClean(VbendInMesh) || (vertsVBendIn.length() == 0) || !isBendInMeshClean ){
		// Get VBEND mesh
		MDataHandle VbendInMeshData = data.inputValue(VbendInMesh, &status);
		if (VbendInMeshData.type() != MFnData::kMesh) {	MItGeometry iterVBendIn(bendInMeshData, groupId, false); iterVBendIn.allPositions(vertsVBendIn); }
		else {											MItGeometry iterVBendIn(VbendInMeshData, groupId, false); iterVBendIn.allPositions(vertsVBendIn); }
	}
	if (!data.isClean(UbendOutMesh) || (vertsUBendOut.length() == 0) || !isBendOutMeshClean ){
		// Get USTRAIGHTEN mesh
		MDataHandle UbendOutMeshData = data.inputValue(UbendOutMesh, &status);
		if (UbendOutMeshData.type() != MFnData::kMesh) {	MItGeometry iterUBendOut(bendOutMeshData, groupId, false); iterUBendOut.allPositions(vertsUBendOut); }
		else {												MItGeometry iterUBendOut(UbendOutMeshData, groupId, false); iterUBendOut.allPositions(vertsUBendOut); }
	}
	if (!data.isClean(VbendOutMesh) || (vertsVBendOut.length() == 0) || !isBendOutMeshClean ){
		// Get VSTRAIGHTEN mesh
		MDataHandle VbendOutMeshData = data.inputValue(VbendOutMesh, &status);
		if (VbendOutMeshData.type() != MFnData::kMesh) {	MItGeometry iterVBendOut(bendOutMeshData, groupId, false); iterVBendOut.allPositions(vertsVBendOut); }
		else {												MItGeometry iterVBendOut(VbendOutMeshData, groupId, false); iterVBendOut.allPositions(vertsVBendOut); }
	}

if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting the UV Direction meshes. \n "; clo = clock(); }

	//############################## Get all ATTRIBUTES ##############################

	// Get stretchRelax Attr
	MDataHandle stretchRelaxData = data.inputValue(stretchRelax, &status);
	MCheckStatus(status, "fStretch: Cannot get the stretchRelax attr.");

	// Get squashRelax Attr
	MDataHandle squashRelaxData = data.inputValue(squashRelax, &status);
	MCheckStatus(status, "fStretch: Cannot get the squashRelax attr.");

	// Get bendInRelax Attr
	MDataHandle bendInRelaxData = data.inputValue(bendInRelax, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendInRelax attr.");

	// Get bendOutRelax Attr
	MDataHandle bendOutRelaxData = data.inputValue(bendOutRelax, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendOutRelax attr.");

	// Get scaleFactor Attr
	MDataHandle scaleFactorData = data.inputValue(scaleFactor, &status);
	MCheckStatus(status, "fStretch: Cannot get the scaleFactor attr.");

	// Get stretchGrowShrink Attr
	MDataHandle stretchGrowShrinkData = data.inputValue(stretchGrowShrink, &status);
	MCheckStatus(status, "fStretch: Cannot get the stretchGrowShrink attr.");

	// Get squashGrowShrink Attr
	MDataHandle squashGrowShrinkData = data.inputValue(squashGrowShrink, &status);
	MCheckStatus(status, "fStretch: Cannot get the squashGrowShrink attr.");

	// Get tensionPostSmooth Attr
	MDataHandle tensionPostSmoothData = data.inputValue(tensionPostSmooth, &status);
	MCheckStatus(status, "fStretch: Cannot get the tensionPostSmooth attr.");

	// Get bendInGrowShrink Attr
	MDataHandle bendInGrowShrinkData = data.inputValue(bendInGrowShrink, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendInGrowShrink attr.");

	// Get bendOutGrowShrink Attr
	MDataHandle bendOutGrowShrinkData = data.inputValue(bendOutGrowShrink, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendOutGrowShrink attr.");

	// Get stretchGrowShrink Attr
	MDataHandle anglePostSmoothData = data.inputValue(anglePostSmooth, &status);
	MCheckStatus(status, "fStretch: Cannot get the anglePostSmooth attr.");


	// Get tensionEnvelope Attr
	MDataHandle tensionEnvelopeData = data.inputValue(tensionEnvelope, &status);
	MCheckStatus(status, "fStretch: Cannot get the tensionEnvelope attr.");

	// Get squashMultiplier Attr
	MDataHandle squashMultiplierData = data.inputValue(squashMultiplier, &status);
	MCheckStatus(status, "fStretch: Cannot get the squashMultiplier attr.");

	// Get stretchMultiplier Attr
	MDataHandle stretchMultiplierData = data.inputValue(stretchMultiplier, &status);
	MCheckStatus(status, "fStretch: Cannot get the stretchMultiplier attr.");

	// Get the tensionUDirection
	MDataHandle tensionUDirectionData = data.inputValue(tensionUDirection, &status);
	MCheckStatus(status, "fStretch: Cannot get the tensionUDirection attr.");

	// Get the tensionVDirection
	MDataHandle tensionVDirectionData = data.inputValue(tensionVDirection, &status);
	MCheckStatus(status, "fStretch: Cannot get the tensionVDirection attr.");

	// Get tensionSmooth Attr
	MDataHandle tensionSmoothData = data.inputValue(tensionSmooth, &status);
	MCheckStatus(status, "fStretch: Cannot get the tensionSmooth attr.");

	// Get displayWarnings Attr
	MDataHandle displayWarningsData = data.inputValue( displayWarnings, &status );
	MCheckStatus(status, "fStretch: Cannot get the displayWarnings attr.");

	// Get drawColorPerVertex Attr
	MDataHandle drawColorPerVertexData = data.inputValue( drawColorPerVertex, &status );
	MCheckStatus(status, "fStretch: Cannot get the drawColorPerVertex attr.");

	// Get colorDisplay Attr
	MDataHandle colorDisplayData = data.inputValue( colorDisplay, &status );
	MCheckStatus(status, "fStretch: Cannot get the colorDisplay attr.");

	// Get stretchRange Attr
	MDataHandle stretchRangeData = data.inputValue(stretchRange, &status);
	MCheckStatus(status, "fStretch: Cannot get the stretchRange attr.");

	// Get squashRange Attr
	MDataHandle squashRangeData = data.inputValue(squashRange, &status);
	MCheckStatus(status, "fStretch: Cannot get the squashRange attr.");

	// Get tensionColorContrast Attr
	MDataHandle tensionColorContrastData = data.inputValue(tensionColorContrast, &status);
	MCheckStatus(status, "fStretch: Cannot get the tensionColorContrast attr.");

	// Get angleColorContrast Attr
	MDataHandle angleColorContrastData = data.inputValue(angleColorContrast, &status);
	MCheckStatus(status, "fStretch: Cannot get the angleColorContrast attr.");

	// Get forceColorRefresh Attr
	MDataHandle forceColorRefreshData = data.inputValue(forceColorRefresh, &status);
	MCheckStatus(status, "fStretch: Cannot get the forceColorRefresh attr.");

	////////// Angles attributes /////////

	// Get forceColorRefresh Attr
	MDataHandle angleEnvelopeData = data.inputValue(angleEnvelope, &status);
	MCheckStatus(status, "fStretch: Cannot get the angleEnvelope attr.");

	// Get angleSmooth Attr
	MDataHandle angleSmoothData = data.inputValue(angleSmooth, &status);
	MCheckStatus(status, "fStretch: Cannot get the angleSmooth attr.");

	// Get bendIn Attr
	MDataHandle bendInData = data.inputValue(bendIn, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendIn attr.");

	// Get bendInRange Attr
	MDataHandle bendInRangeData = data.inputValue(bendInRange, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendInRange attr.");

	// Get bendOut Attr
	MDataHandle bendOutData = data.inputValue(bendOut, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendOut attr.");

	// Get bendOutRange Attr
	MDataHandle bendOutRangeData = data.inputValue(bendOutRange, &status);
	MCheckStatus(status, "fStretch: Cannot get the bendOutRange attr.");

	// Get angleUDirection Attr
	MDataHandle angleUDirectionData = data.inputValue(angleUDirection, &status);
	MCheckStatus(status, "fStretch: Cannot get the angleUDirection attr.");

	// Get angleVDirection Attr
	MDataHandle angleVDirectionData = data.inputValue(angleVDirection, &status);
	MCheckStatus(status, "fStretch: Cannot get the angleVDirection attr.");
	
	// Get counterSharpAreas Attr
	MRampAttribute counterSharpAreasData(thisMObject(),counterSharpAreas, &status); MCheckStatus(status, "fStretch: Cannot get the counterSharpAreas attr.");

	// Get tensionFalloff Attr
	MRampAttribute tensionFalloffData(thisMObject(),tensionFalloff, &status); MCheckStatus(status, "fStretch: Cannot get the tensionFalloff attr.");

	// Get angleFalloff Attr
	MRampAttribute angleFalloffData(thisMObject(),angleFalloff, &status); MCheckStatus(status, "fStretch: Cannot get the angleFalloff attr.");

	////////// Wrap attributes ////////////

	// Get wrap Attr
	MDataHandle wrapData = data.inputValue(wrap, &status);
	MCheckStatus(status, "fStretch: Cannot get the wrap attr.");

	// Get wrapSmoothRadius Attr
	MDataHandle wrapSmoothRadiusData = data.inputValue(wrapSmoothRadius, &status);
	MCheckStatus(status, "fStretch: Cannot get the wrapSmoothRadius attr.");

	// Get wrapItemsPerRadius Attr
	MDataHandle wrapItemsPerRadiusData = data.inputValue(wrapItemsPerRadius, &status);
	MCheckStatus(status, "fStretch: Cannot get the wrapItemsPerRadius attr.");

	// Get wrapCentralFactor Attr
	MDataHandle wrapCentralFactorData = data.inputValue(wrapCentralFactor, &status);
	MCheckStatus(status, "fStretch: Cannot get the wrapCentralFactor attr.");

	// Get wrapOneTriPerFace Attr
	MDataHandle wrapOneTriPerFaceData = data.inputValue(wrapOneTriPerFace, &status);
	MCheckStatus(status, "fStretch: Cannot get the wrapOneTriPerFace attr.");

	//////////////////// Ramp attributes //////////////////////

	// Curve Ramps
	MRampAttribute stretchFalloffData(thisMObject(),stretchFalloff, &status); MCheckStatus(status, "fStretch: Cannot get the falloff attr.");
	MRampAttribute squashFalloffData(thisMObject(),squashFalloff, &status); MCheckStatus(status, "fStretch: Cannot get the falloff attr.");
	MRampAttribute bendInFalloffData(thisMObject(),bendInFalloff, &status); MCheckStatus(status, "fStretch: Cannot get the falloff attr.");
	MRampAttribute bendOutFalloffData(thisMObject(),bendOutFalloff, &status); MCheckStatus(status, "fStretch: Cannot get the falloff attr.");

	// Color Ramps
	MRampAttribute tensionColorRampData(thisMObject(),tensionColorRamp, &status); MCheckStatus(status, "fStretch: Cannot get the tensionColorRamp attr.");
	MRampAttribute angleColorRampData(thisMObject(),angleColorRamp, &status); MCheckStatus(status, "fStretch: Cannot get the tensionColorRamp attr.");

	// Get UseAllAvailableProcessors Attr
	MDataHandle UseAllAvailableProcessorsData = data.inputValue( UseAllAvailableProcessors, &status );
	MCheckStatus(status, "fStretch: Cannot get the UseAllAvailableProcessors attr.");

	// Get NumberOfProcessorsToUseData Attr
	MDataHandle NumberOfProcessorsToUseData = data.inputValue( NumberOfProcessorsToUse, &status );
	MCheckStatus(status, "fStretch: Cannot get the NumberOfProcessorsToUse attr.");

	// Set the number of processors to use

	#ifndef _DEBUG
	#ifdef _OPENMP
	int iCPU;
	if (UseAllAvailableProcessorsData.asBool() == 1){
		// Get the number of processors in this system
		iCPU = omp_get_num_procs();
	} else {
		iCPU = NumberOfProcessorsToUseData.asInt();
	}
	// Now set the number of threads
	omp_set_num_threads(iCPU);
	#endif
	#endif

if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting the node's attributes. \n "; clo = clock(); }
	//############################## Create mesh/verts ITERATORS ##############################

	// Create an iteration variable for OUTPUT
	MItGeometry iter(outputData, groupId, false);
	MPointArray verts;
	iter.allPositions(verts);

	int nPoints = verts.length();

	MItGeometry iterIn(inputData, groupId, false);
	MPointArray vertsIn;
	iterIn.allPositions(vertsIn);

	// Create the VERTEX ITERATOR
	MObject mesh = outputData.asMesh();
	MItMeshVertex vertexIter(mesh, &status);
if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Creating the iterators. \n ";clo = clock(); }
	// Create the VERTEX ITERATOR for restMesh (for getting normals of the rest mesh... for angle-based deformation that takes normals)
	MObject restMesh = restData.asMesh();
	MItMeshVertex vertexIterRest(restMesh, &status); 
if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Creating the iterator for the normals. \n ";clo = clock(); }
	// Create EDGE/FACE ITERATOR

	MItMeshEdge edgeIter(mesh);
	int nEdges = edgeIter.count();

	MItMeshPolygon polyIter(mesh);
	int nPoly = polyIter.count();

	///////////////////////////////////////////// UV DIRECTION WARNING //////////////////////////////////////

	if (displayWarningsData.asBool() == true){
		if (firstComputeDone == 1){
			if ( (lastTensionUDirection != tensionUDirectionData.asFloat()) || (lastTensionVDirection != tensionVDirectionData.asFloat()) || (lastAngleUDirection != angleUDirectionData.asFloat()) || (lastAngleVDirection != angleVDirectionData.asFloat()) ){
				time_t currentTime;
				time(&currentTime);

				if (currentTime > UVDirectionTime) {
					MGlobal::executeCommandStringResult("fStretch_window(\"UVDirection\");", false, false, NULL);
					UVDirectionTime = currentTime + 300;
				}
			}
		}
		else { time(&UVDirectionTime); }
	}

	//

	MFnMesh colorMesh(mesh); // This will be used for drawing colors and for getting normals for angle-based deformations
	MFnMesh MFnMeshRest(restMesh); // This will be used for getting normals for angle-based deformations

if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting MFnMesh. \n ";clo = clock(); }

	//############################## Check if all the geometry have the same number of points ##############################

	if (vertsRest.length() != nPoints) {		MGlobal::displayError("fStretch: The restMesh doesn't have the same vertex number as the original mesh.\n");		return MStatus::kFailure; }
	if (vertsSquash.length() != nPoints) {		MGlobal::displayError("fStretch: The squashMesh doesn't have the same vertex number as the original mesh.\n");		return MStatus::kFailure; }
	if (vertsStretch.length() != nPoints) {		MGlobal::displayError("fStretch: The stretchMesh doesn't have the same vertex number as the original mesh.\n");		return MStatus::kFailure; }
	if (vertsBendIn.length() != nPoints) {		MGlobal::displayError("fStretch: The bendInMesh doesn't have the same vertex number as the original mesh.\n");		return MStatus::kFailure; }
	if (vertsBendOut.length() != nPoints) {	MGlobal::displayError("fStretch: The bendOutMesh doesn't have the same vertex number as the original mesh.\n");	return MStatus::kFailure; }

	//############################## Create an EMPTY MATRIX to store the CONNECTED VERTEX of each vertex. ##############################

	// Check if the matrix already exists before creating it. Also, recreate it if the mesh has changed.
	// If the number of vertex/edge/face has changed, or it's the first time the plugin compute -> create all our matrices !
	int createMatrix = 0;

	if ( isMatrixCreated == 0 ) { createMatrix = 1; }
	
	if (lastNumberOfVertex != nPoints) { createMatrix = 1; }
	if (lastNumberOfEdges != nEdges) { createMatrix = 1; }
	if (lastNumberOfPoly != nPoly) { createMatrix = 1; }

	// See if wrap values changed. If they did, then we will recompute the matrix
	if (lastWrapSmoothRadius != wrapSmoothRadiusData.asInt()) { createMatrix = 1; }
	if (lastWrapItemsPerRadius != wrapItemsPerRadiusData.asFloat()) { createMatrix = 1; }
	if (lastWrapCentralFactor != wrapCentralFactorData.asFloat()) { createMatrix = 1; }
	if (lastWrapOneTriPerFace != wrapOneTriPerFaceData.asBool()) { createMatrix = 1; }
	if (lastWrapData != wrapData.asBool()) { createMatrix = 1; }

if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Verifying the connected meshes' number of points and comparing some attributes. \n ";clo = clock(); }

	if (createMatrix == 1){

		isMatrixCreated = 1;

		// Free up arrays and matrices to prevent memory leaks. Do this when we create our matrices EXCEPT the first time.

		if (firstComputeDone == 1){	// If it's not the first compute, flush all rows in our matrix
			if (stretchArrayExists == 1) { delete[] stretchArray; stretchArrayExists = 0; }
			if (tensionSmoothArrayExists == 1){ delete[] tensionSmoothArray; tensionSmoothArrayExists = 0; }

			for (int e=0; e<lastNumberOfVertex; e++){
				if (vertexTrisMatrixExists == 1) { delete[] vertexTrisMatrix[e]; }
				if (connectedVertsMatrixExists == 1) { delete[] connectedVertsMatrix[e]; }
				if (UVDirectionMatrixExists == 1) { delete[] UVDirectionMatrix[e]; }
			}
			if (vertexTrisMatrixExists == 1) { delete[] vertexTrisMatrix; vertexTrisMatrixExists = 0; }
			if (connectedVertsMatrixExists == 1) { delete[] connectedVertsMatrix; connectedVertsMatrixExists = 0; }
			if (UVDirectionMatrixExists == 1) { delete[] UVDirectionMatrix; UVDirectionMatrixExists = 0; }
		}

		int nrows = nPoints; int ncols = 0;

		// Create stretchArray
		stretchArray = new float[nPoints];
		for (int i=0; i<nPoints; i++) {
			stretchArray[i] = 0;	// Initialize all elements to zero.
		}
		stretchArrayExists = 1;

		// Create tensionSmoothArray
		tensionSmoothArray = new float[nPoints];
		for (int i=0; i<nPoints; i++) {
			tensionSmoothArray[i] = 0;	// Initialize all elements to zero.
		}
		tensionSmoothArrayExists = 1;

		// #################################################################### vertexTrisMatrix ####################################################################

		if (wrapData.asBool() == 1){
			// Create vertexTrisMatrix matrix and put all our vertex infos inside it.
			//
			// OUR MATRIX stores all the data in that format:
			//
			// vertexTrisMatrix[vertex][0] = number of triangles that we use for this vertex
			//
			// vertexTrisMatrix[vertex][1] = triangle's vertex 1
			// vertexTrisMatrix[vertex][2] = triangle's vertex 2
			// vertexTrisMatrix[vertex][3] = triangle's vertex 3
			// vertexTrisMatrix[vertex][4] = U
			// vertexTrisMatrix[vertex][5] = V
			// vertexTrisMatrix[vertex][6] = N
			// ... and it goes on for each tri ( element 0 does not repeat )		
			// U V N is our relation from our restMesh and sculptedMesh, we will record that later and use it to wrap our mesh once it has moved.

			vertexTrisMatrix = new matrixFloatPtr[nrows];
			vertexTrisMatrixExists = 1;
			vertexIter.reset();

			for (int i = 0; i < nrows; i++) {

				// Find the faces next to our vertex
				MIntArray connectedFaces;
				vertexIter.getConnectedFaces(connectedFaces);

				// Filter those face with ratioPerRadius attribute
				MIntArray connectedFacesFiltered; connectedFacesFiltered.clear();
				MIntArray tempTrisPerRadius; tempTrisPerRadius.clear();
				tempTrisPerRadius = trisPerRadius( 0, connectedFaces.length(), wrapSmoothRadiusData.asInt(), wrapItemsPerRadiusData.asFloat(), wrapCentralFactorData.asFloat() );
				// put that in facesArrayFiltered
				for (int tpr=0; tpr<tempTrisPerRadius.length(); tpr++){
					connectedFacesFiltered.append(connectedFaces[tempTrisPerRadius[tpr]]);
				}

				int faceCount;
				vertexIter.numConnectedFaces(faceCount);

				// Grow our selection of faces for our current vertex.
				if (wrapSmoothRadiusData.asInt() > 1){
					
					for (int r=1; r<wrapSmoothRadiusData.asInt(); r++){

						MIntArray facesToAdd; facesToAdd.clear();
						// loop trough each of our connected faces and for each one, get their connect faces (in other word, grow faces)
						for (int f=0; f<connectedFaces.length(); f++){
							MIntArray faces; faces.clear();
							int prevIndex = 0;
							polyIter.setIndex(connectedFaces[f], prevIndex);
							polyIter.getConnectedFaces(faces);
							// Add the face to facesToAdd SEULEMENT SI PAS DEJA DEDANS !!!
							for (int fa=0; fa<(int)faces.length(); fa++){
								int alreadyIn = 0;
								// Verify if it's already in the array before adding it.
								for (int fta=0; fta<(int)facesToAdd.length(); fta++){
									if (faces[fa] == facesToAdd[fta]) { alreadyIn = 1; }
								}
								// Add it
								if (alreadyIn == 0) {
									facesToAdd.append(faces[fa]);
								}
							}
						}
						MIntArray addOnlyThose; addOnlyThose.clear();
						// Find the faces in facesToAdd that are not already in connectedFaces
						for (int fta=0; fta<facesToAdd.length(); fta++){
							// Loop trough each faces to add
							int alreadyIn = 0;
							for (int cf=0; cf<connectedFaces.length(); cf++){
								// Look if it's already in connectedFaces
								if (facesToAdd[fta] == connectedFaces[cf]) { alreadyIn = 1; }
							}
							if (alreadyIn == 0){ addOnlyThose.append(facesToAdd[fta]); }
						}
						// Add facesToAdd to connectedFaces..
						for (int each = 0; each<(int)addOnlyThose.length(); each++){
							connectedFaces.append(addOnlyThose[each]);
						}
						// Add the faces to our connectedFacesFiltered
						tempTrisPerRadius.clear();
						tempTrisPerRadius = trisPerRadius( r, connectedFaces.length(), wrapSmoothRadiusData.asInt(), wrapItemsPerRadiusData.asFloat(), wrapCentralFactorData.asFloat() );
						// put that in facesArrayFiltered
						for (int tpr=0; tpr<tempTrisPerRadius.length(); tpr++){
							connectedFacesFiltered.append(connectedFaces[tempTrisPerRadius[tpr]]);
						}

					}			
				}

				connectedFaces = connectedFacesFiltered;

				// Build vtxPerFaceList by finding the vertex of each of our faces
				MIntArray vtxPerFaceList; vtxPerFaceList.clear();
				MIntArray facesArray; facesArray.clear();
				
				for (int f=0; f<(int)connectedFaces.length(); f++){
					MIntArray faceVertices; faceVertices.clear();
					polyIter.setIndex(connectedFaces[f],connectedFaces[f]);
					polyIter.getVertices(faceVertices);

					// now that we know the number of vertex that our current face has, add that to our vtxPerFaceList array
					vtxPerFaceList.append((int)faceVertices.length());

					// add each vertex in facesArray
					for (int v=0; v<(int)faceVertices.length(); v++){
						facesArray.append(faceVertices[v]);
					}
				}

				// Sort them and decompose them into tris.

				MIntArray tris; tris.clear();

				tris = orderFacesByNeighboors_decomposeIntoTriArray(facesArray, vtxPerFaceList, wrapOneTriPerFaceData.asInt());

				// Put them in the matrix.
				vertexTrisMatrix[i] = NULL;
				vertexTrisMatrix[i] = new float[ ((int)tris.length()*2+1) ];
				
				// Put the number of triangle into the first element
				vertexTrisMatrix[i][0] = (float)(tris.length() /3);

				// Put each tris inside the matrix
				for (int triIndex = 0; triIndex<vertexTrisMatrix[i][0]; triIndex++){
					vertexTrisMatrix[i][((triIndex*6+1) + 0)] = (float)tris[triIndex*3];
					vertexTrisMatrix[i][((triIndex*6+1) + 1)] = (float)tris[triIndex*3+1];
					vertexTrisMatrix[i][((triIndex*6+1) + 2)] = (float)tris[triIndex*3+2];
				}
				vertexIter.next();
			}
		}

		// #################################################################### connectedVertsMatrix ####################################################################
		// Create the connectedVertsMatrix and the UVDirectionMatrix

		vertexIter.reset();

		connectedVertsMatrix = new matrixPtr[nrows];
		UVDirectionMatrix = new matrixFloatPtr[nrows];

		for (int i = 0; i < nrows; i++){
			connectedVertsMatrix[i] = NULL;
			UVDirectionMatrix[i] = NULL;
		}
		connectedVertsMatrixExists = 1;
		UVDirectionMatrixExists = 1;

		for (int i = 0; i < nrows; i++) {
			// Calculate the number of edges connected to the vertex
			MIntArray connectedVertexIndices;
			connectedVertexIndices.clear();
			vertexIter.getConnectedVertices(connectedVertexIndices);
			vertexIter.next();
			ncols = (connectedVertexIndices.length() + 1);

			connectedVertsMatrix[i] = new int[ ncols ];
			UVDirectionMatrix[i] = new float[ ((ncols-1)*2) ];

			for (int y=0; y < ncols; y++) { connectedVertsMatrix[i][y] = -1; }
		}

		//############################## FILL the MATRIX with the CONNECTED VERTEX ##############################

		vertexIter.reset();

		for (; !vertexIter.isDone(); vertexIter.next()) {
			MIntArray connectedVertexIndices;
			connectedVertexIndices.clear();
			vertexIter.getConnectedVertices(connectedVertexIndices);
	
			connectedVertsMatrix[vertexIter.index()][0] = connectedVertexIndices.length();
			for (int littleI = 0; littleI < (int)connectedVertexIndices.length(); littleI++){
				connectedVertsMatrix[vertexIter.index()][(littleI+1)] = connectedVertexIndices[littleI];

			}

		}
		////////////////////////////// UV Direction Matrix //////////////////////////
		vertexIter.reset();
		for (int v=0; v<nPoints; v++) {
			int prevIndex=0;
			vertexIter.setIndex(v,prevIndex);
			float2 vtxUV;
			vertexIter.getUV(vtxUV);

			// Iterate over the number of connected vertex and fill in uv values
			for (int e=0; e<connectedVertsMatrix[v][0]; e++){
				// Get the UV of each connected VTX
				int prevIndex;
				vertexIter.setIndex(connectedVertsMatrix[v][e+1] , prevIndex);
				float2 eachConVtxUV;
				vertexIter.getUV(eachConVtxUV);

				float U = abs(eachConVtxUV[0] - vtxUV[0]);
				float V = abs(eachConVtxUV[1] - vtxUV[1]);

				U = noZero(U);
				V = noZero(V);

				// Normalize so that if we add all UV of all neighboors edges of our vtx it gives 1
				float UV = U+V;
				float edgeCount = connectedVertsMatrix[v][0];
				U = U * ( (1.0/noZero(edgeCount)) / noZero((UV)) );
				V = V * ( (1.0/noZero(edgeCount)) / noZero((UV)) );

				UVDirectionMatrix[v][(e*2)] = abs(U);
				UVDirectionMatrix[v][(e*2)+1] = abs(V);
			}

		}

	}

	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Creating cached data. \n ";clo = clock(); }

	// Record the wrap values so we can later check if they changed
	lastWrapSmoothRadius = wrapSmoothRadiusData.asInt();
	lastWrapItemsPerRadius = wrapItemsPerRadiusData.asFloat();
	lastWrapCentralFactor = wrapCentralFactorData.asFloat();
	lastWrapOneTriPerFace = wrapOneTriPerFaceData.asBool();
	lastWrapData = wrapData.asBool();

	lastUseAllAvailableProcessors = UseAllAvailableProcessorsData.asBool();
	lastNumberOfProcessorsToUse = NumberOfProcessorsToUseData.asInt();

	lastTensionUDirection = tensionUDirectionData.asFloat();
	lastTensionVDirection = tensionVDirectionData.asFloat();
	lastAngleUDirection = angleUDirectionData.asFloat();
	lastAngleVDirection = angleVDirectionData.asFloat();

	lastNumberOfVertex = nPoints;
	lastNumberOfEdges = nEdges;
	lastNumberOfPoly = nPoly;

	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Recording values. \n ";clo = clock(); }

	/////////////////////////////// Get grow shrink / relax / range paint weights ////////////////////////////////////

	float* paintStretchGrowShrinkSmoothArray = NULL;
	float* paintSquashGrowShrinkSmoothArray = NULL;
	float* paintBendInGrowShrinkSmoothArray = NULL;
	float* paintBendOutGrowShrinkSmoothArray = NULL;
	float* paintTensionRangeArray = NULL;
	float* paintAngleRangeArray = NULL;
	float* paintStretchRelaxArray = NULL;
	float* paintSquashRelaxArray = NULL;
	float* paintBendInRelaxArray = NULL;
	float* paintBendOutRelaxArray = NULL;

	paintStretchGrowShrinkSmoothArray = new float[nPoints];
	paintSquashGrowShrinkSmoothArray = new float[nPoints];
	paintBendInGrowShrinkSmoothArray = new float[nPoints];
	paintBendOutGrowShrinkSmoothArray = new float[nPoints];
	paintTensionRangeArray = new float[nPoints];
	paintAngleRangeArray = new float[nPoints];
	paintStretchRelaxArray = new float[nPoints];
	paintSquashRelaxArray = new float[nPoints];
	paintBendInRelaxArray = new float[nPoints];
	paintBendOutRelaxArray = new float[nPoints];

	int pstgsaExists = paintStretchGrowShrinkarrayHandle.elementCount();
	int psqgsaExists = paintSquashGrowShrinkarrayHandle.elementCount();
	int pbigsaExists = paintBendInGrowShrinkarrayHandle.elementCount();
	int pbogsaExists = paintBendOutGrowShrinkarrayHandle.elementCount();
	int ptraExists = paintTensionRangearrayHandle.elementCount();
	int paraExists = paintAngleRangearrayHandle.elementCount();
	int pstraExists = paintStretchRelaxarrayHandle.elementCount();
	int psraExists = paintSquashRelaxarrayHandle.elementCount();
	int pbiraExists = paintBendInRelaxarrayHandle.elementCount();
	int pboraExists = paintBendOutRelaxarrayHandle.elementCount();

	for(int i=0; i<nPoints; i++) {
		paintStretchGrowShrinkarrayHandle.jumpToArrayElement(i);
		paintSquashGrowShrinkarrayHandle.jumpToArrayElement(i);
		paintBendInGrowShrinkarrayHandle.jumpToArrayElement(i);
		paintBendOutGrowShrinkarrayHandle.jumpToArrayElement(i);
		paintTensionRangearrayHandle.jumpToArrayElement(i);
		paintAngleRangearrayHandle.jumpToArrayElement(i);
		paintStretchRelaxarrayHandle.jumpToArrayElement(i);
		paintSquashRelaxarrayHandle.jumpToArrayElement(i);
		paintBendInRelaxarrayHandle.jumpToArrayElement(i);
		paintBendOutRelaxarrayHandle.jumpToArrayElement(i);
		if (pstgsaExists != 0) { paintStretchGrowShrinkSmoothArray[i] = paintStretchGrowShrinkarrayHandle.inputValue().asFloat(); } else {	paintStretchGrowShrinkSmoothArray[i] = 1; }
		if (psqgsaExists != 0) { paintSquashGrowShrinkSmoothArray[i] =	paintSquashGrowShrinkarrayHandle.inputValue().asFloat(); } else {	paintSquashGrowShrinkSmoothArray[i] = 1; }
		if (pbigsaExists != 0) { paintBendInGrowShrinkSmoothArray[i] =	paintBendInGrowShrinkarrayHandle.inputValue().asFloat(); } else {	paintBendInGrowShrinkSmoothArray[i] = 1; }
		if (pbogsaExists != 0) { paintBendOutGrowShrinkSmoothArray[i] = paintBendOutGrowShrinkarrayHandle.inputValue().asFloat(); } else {	paintBendOutGrowShrinkSmoothArray[i] = 1; }
		if (ptraExists != 0)  { paintTensionRangeArray[i] =				paintTensionRangearrayHandle.inputValue().asFloat(); } else {		paintTensionRangeArray[i] = 1; }
		if (paraExists != 0)  { paintAngleRangeArray[i] =				paintAngleRangearrayHandle.inputValue().asFloat(); } else {			paintAngleRangeArray[i] = 1; }
		if (pstraExists != 0) { paintStretchRelaxArray[i] =				paintStretchRelaxarrayHandle.inputValue().asFloat(); } else {		paintStretchRelaxArray[i] = 1; }
		if (psraExists  != 0) { paintSquashRelaxArray[i] =				paintSquashRelaxarrayHandle.inputValue().asFloat(); } else {		paintSquashRelaxArray[i] = 1; }
		if (pbiraExists != 0) { paintBendInRelaxArray[i] =				paintBendInRelaxarrayHandle.inputValue().asFloat(); } else {		paintBendInRelaxArray[i] = 1; }
		if (pboraExists != 0) { paintBendOutRelaxArray[i] =				paintBendOutRelaxarrayHandle.inputValue().asFloat(); } else {		paintBendOutRelaxArray[i] = 1; }
	}

	/////////////////////////////// Get UV Direction paint weights ////////////////////////////////////
	// Decompose the weights into local array to work with it multithreaded

	float* paintTensionDirectionArray = NULL;
	float* paintAngleDirectionArray = NULL;

	paintTensionDirectionArray = new float[nPoints];
	paintAngleDirectionArray = new float[nPoints];

	int ptdaExists = paintTensionDirectionarrayHandle.elementCount();
	int padaExists = paintAngleDirectionarrayHandle.elementCount();

	for(int i=0; i<nPoints; i++) {
		paintTensionDirectionarrayHandle.jumpToArrayElement(i);
		paintAngleDirectionarrayHandle.jumpToArrayElement(i);

		if (ptdaExists != 0) { paintTensionDirectionArray[i] = paintTensionDirectionarrayHandle.inputValue().asFloat(); } else { paintTensionDirectionArray[i] = .5; }
		if (padaExists != 0) { paintAngleDirectionArray[i] = paintAngleDirectionarrayHandle.inputValue().asFloat(); } else { paintAngleDirectionArray[i] = .5; }
	}

	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting painted weights. \n ";clo = clock(); }

	//###################################################################################
	//############################## Calculate the TENSION ##############################
	//###################################################################################

	float2* tensionDirectionRatio = NULL;
	tensionDirectionRatio = new float2[nPoints];

		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for(int i=0; i<nPoints; i++) {

			// Find the stretch between the vertex

			float stretch;
			float cStretch; // Current stretch
			float cRestStretch;

			int nEdges;
			stretch = 0; float restStretch = 0;
			float directionIndice; float directionIndiceU = 0; float directionIndiceV = 0;
			for ( nEdges = 0; nEdges < connectedVertsMatrix[i][0]; nEdges++ ){

				cRestStretch = (vertsRest[i].distanceTo(vertsRest[connectedVertsMatrix[i][(nEdges+1)]]));
				cStretch = vertsIn[i].distanceTo(vertsIn[connectedVertsMatrix[i][(nEdges+1)]]);
				cStretch /= noZero(abs(scaleFactorData.asFloat()));

				float distanceDifference = (( cStretch / noZero(cRestStretch) ) - 1);

				float U = UVDirectionMatrix[i][(nEdges*2)] * tensionUDirectionData.asFloat() * convertPaintedU(paintTensionDirectionArray[i]);
				float V = UVDirectionMatrix[i][(nEdges*2)+1] * tensionVDirectionData.asFloat() * convertPaintedV(paintTensionDirectionArray[i]);

				directionIndiceU += abs(distanceDifference) * U;
				directionIndiceV += abs(distanceDifference) * V;

				// Integrate our UV direction into the tension
				stretch += distanceDifference * (U+V);
			}

			// Put our U V direction indices in our array so we can later use it to know if we have to take the tensionU mesh, tensionV mesh, or tension mesh
			// We need to add this offset to the direction indices because when there's no tension at all, it's hard to determined that's it's half/half U and V ways. values get very small.
			directionIndiceU += .00001; directionIndiceV += .00001;
			tensionDirectionRatio[i][0] = directionIndiceU / noZero((directionIndiceU+directionIndiceV));
			tensionDirectionRatio[i][1] = directionIndiceV / noZero((directionIndiceU+directionIndiceV));

			// Multiply that value with the falloff and put the ranges

			// Stretch
			if (stretch > 0){

				stretchFalloffData.getValueAtPosition(stretch/noZero((stretchRangeData.asFloat() * paintTensionRangeArray[i])),stretch,&status);
				stretchArray[i] = stretch;
			}
			else{
				// Squash
				squashFalloffData.getValueAtPosition(-stretch/noZero((squashRangeData.asFloat() * paintTensionRangeArray[i])),stretch,&status);
				stretchArray[i] = -stretch;
			}

		}
	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Calculating the tension.\n ";clo = clock(); }
		//############################## Smooth the TENSION ##############################

		// Decompose the tensionSmooth array into local array to work with it multithreaded

		float* paintTensionSmoothArray = NULL;
		paintTensionSmoothArray = new float[nPoints];
		int ptsaExists = tensionSmoothPaintarrayHandle.elementCount();
			for(int i=0; i<nPoints; i++) {
				tensionSmoothPaintarrayHandle.jumpToArrayElement(i);
				if (ptsaExists != 0) { paintTensionSmoothArray[i] = tensionSmoothPaintarrayHandle.inputValue().asFloat(); } else { paintTensionSmoothArray[i] = 1; }
			}

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting tension's painted weights. \n ";clo = clock(); }

		// Now tensionSmooth it !

		// If we want to multi thread the tension smooth we have to make it work parallel. right now stretchArray is reading and write at the same time so it only work serialy
		if (tensionSmoothData.asInt() > 0){

			for (int n = 0;n<tensionSmoothData.asInt();n++){
				int nEdges;
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for private(nEdges)
				#endif
				#endif
				for(int i=0; i<nPoints; i++) {

					if ( (paintTensionSmoothArray[i] * tensionSmoothData.asInt() ) > n){

						float stretch = 0;
						for ( nEdges = 0; nEdges < connectedVertsMatrix[i][0]; nEdges++ ){

							stretch += stretchArray[connectedVertsMatrix[i][(nEdges+1)]];
						}
						tensionSmoothArray[i] = ( stretch / noZero(nEdges) );
					}
					else { tensionSmoothArray[i] = stretchArray[i]; }
				}
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for
				#endif
				#endif
				for (int e=0; e<nPoints; e++){
					stretchArray[e] = tensionSmoothArray[e];
				}
			}
		}
		
		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Smoothing the tension. \n ";clo = clock(); }

		//###################################################################################
		//############################## Calculate the ANGLES ###############################
		//###################################################################################

		float* meshAnglesArray = NULL;
		meshAnglesArray = new float[nPoints];

		float2* angleDirectionRatio = NULL;
		angleDirectionRatio = new float2[nPoints];
		
		float* paintAngleSmoothArray = NULL;
		paintAngleSmoothArray = new float[nPoints];

		float* meshAnglesOriginalArray = NULL;
		meshAnglesOriginalArray = new float[nPoints];

	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Creating arrays. \n ";clo = clock(); }

	if (angleEnvelopeData.asFloat() > 0){

			// Get mesh normals.

			MFloatVectorArray meshNormals;
			
			colorMesh.getVertexNormals(false,meshNormals,MSpace::kObject);

			if ( !isRestMeshClean || (meshNormalsRest.length() == 0) ){
				MFnMeshRest.getVertexNormals(false,meshNormalsRest,MSpace::kObject);
			}

	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting normals. \n ";clo = clock(); }

			bool cropNonQuadsValues = true;

			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for
			#endif
			#endif
			for (int i=0; i<nPoints; i++){
				// We now have our edge and their vertex. Find our points.

				MPoint point_origin = vertsIn[i]; MPoint point_origin_rest = vertsRest[i];
				MPoint point_A = vertsIn[connectedVertsMatrix[i][1]]; MPoint point_A_rest = vertsRest[connectedVertsMatrix[i][1]];
				MPoint point_B = vertsIn[connectedVertsMatrix[i][2]]; MPoint point_B_rest = vertsRest[connectedVertsMatrix[i][2]];
				float directionIndice_A = ( UVDirectionMatrix[i][0] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i])) + ( UVDirectionMatrix[i][1] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i]) );
				float directionIndice_B = ( UVDirectionMatrix[i][2] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i])) + ( UVDirectionMatrix[i][3] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i]) );
				MPoint point_C; MPoint point_C_rest;
				MPoint point_D; MPoint point_D_rest;
				MPoint point_E; MPoint point_E_rest;
				float directionIndice_C;
				float directionIndice_D;
				float directionIndice_E;
				if (connectedVertsMatrix[i][0] >= 3){ point_C = vertsIn[connectedVertsMatrix[i][3]]; point_C_rest = vertsRest[connectedVertsMatrix[i][3]]; directionIndice_C = ( UVDirectionMatrix[i][4] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i]) ) + ( UVDirectionMatrix[i][5] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i]) ); }
				if (connectedVertsMatrix[i][0] >= 4){ point_D = vertsIn[connectedVertsMatrix[i][4]]; point_D_rest = vertsRest[connectedVertsMatrix[i][4]]; directionIndice_D = ( UVDirectionMatrix[i][6] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i]) ) + ( UVDirectionMatrix[i][7] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i])); }
				if (connectedVertsMatrix[i][0] >= 5){ point_E = vertsIn[connectedVertsMatrix[i][5]]; point_E_rest = vertsRest[connectedVertsMatrix[i][5]]; directionIndice_E = ( UVDirectionMatrix[i][8] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i]) ) + ( UVDirectionMatrix[i][1] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i])); }

				// ANGLE FROM POINTS

				int pointCount = connectedVertsMatrix[i][0];
				MVector vector_A = normalizeVector((point_A - point_origin)); MVector vector_A_rest = normalizeVector((point_A_rest - point_origin_rest));
				MVector vector_B = normalizeVector((point_B - point_origin)); MVector vector_B_rest = normalizeVector((point_B_rest - point_origin_rest));

				MVector vector_C; MVector vector_D; MVector vector_E; MVector vector_C_rest; MVector vector_D_rest; MVector vector_E_rest;
				if (pointCount >= 3){ vector_C = normalizeVector((point_C - point_origin)); vector_C_rest = normalizeVector((point_C_rest - point_origin_rest)); }
				if (pointCount >= 4){ vector_D = normalizeVector((point_D - point_origin)); vector_D_rest = normalizeVector((point_D_rest - point_origin_rest)); }
				if (pointCount >= 5){ vector_E = normalizeVector((point_E - point_origin)); vector_E_rest = normalizeVector((point_E_rest - point_origin_rest)); }

				// Now calculate the angles between our vectors given our UV direction

				float resultMultiplier = 100;
				float directionIndiceU = 0; float directionIndiceV = 0; float U; float V;

				float result=0; float result1=0; float result2=0;
				float vectorAngle_AC_rest=0; float vectorAngle_AC=0; float vectorAngle_BD_rest=0; float vectorAngle_BD=0; float counterEffectMultiplier=0;float counterSharpAttr=0; // those variable will also be used as multiplier to counter effect the fact that the hard edges area (or very concave/convex) of a geometry at rest pause lead those area to have higher angle value than other when deformed.
				if (pointCount == 4){ // 4 edges:

					vectorAngle_AC_rest = vectorAngle(vector_A_rest, vector_C_rest, meshNormalsRest[i]);
					vectorAngle_AC = vectorAngle(vector_A, vector_C, meshNormals[i]);
					result1 = (((vectorAngle_AC_rest - vectorAngle_AC)) * (directionIndice_A+directionIndice_C)) * resultMultiplier;
					counterSharpAreasData.getValueAtPosition(abs(vectorAngle_AC_rest*30),counterSharpAttr,&status);

					counterEffectMultiplier = 1-counterSharpAttr; // counter the effects whereas very concave/convex area tends to get higher angle values when deformed
					if (counterEffectMultiplier < 0) { counterEffectMultiplier = 0; }
					result1 *= counterEffectMultiplier;

					// Get the UVS of the edge formed by A and by C
					U =  UVDirectionMatrix[i][0] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i]); V =  UVDirectionMatrix[i][1] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i]);
					U += UVDirectionMatrix[i][4] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i]); V += UVDirectionMatrix[i][5] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i]);
					directionIndiceU += abs(result1) * U;
					directionIndiceV += abs(result1) * V;

					vectorAngle_BD_rest = vectorAngle(vector_B_rest, vector_D_rest, meshNormalsRest[i]);
					vectorAngle_BD = vectorAngle(vector_B, vector_D, meshNormals[i]);
					result2 = (((vectorAngle_BD_rest - vectorAngle_BD)) * (directionIndice_B+directionIndice_D)) * resultMultiplier;
					counterSharpAreasData.getValueAtPosition(abs(vectorAngle_BD_rest*10*3.1416),counterSharpAttr,&status);
					counterEffectMultiplier = 1-counterSharpAttr;
					if (counterEffectMultiplier < 0) { counterEffectMultiplier = 0; }
					result2 *= counterEffectMultiplier;

					// Get the UVS of the edge formed by A and by C
					U =  UVDirectionMatrix[i][2] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i]); V =  UVDirectionMatrix[i][3] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i]);
					U += UVDirectionMatrix[i][6] * angleUDirectionData.asFloat() * convertPaintedU(paintAngleDirectionArray[i]); V += UVDirectionMatrix[i][7] * angleVDirectionData.asFloat() * convertPaintedV(paintAngleDirectionArray[i]);

					directionIndiceU += abs(result2) * U;
					directionIndiceV += abs(result2) * V;

					result = (result1+result2);
				}

				directionIndiceU += .00001;
				directionIndiceV += .00001;
				angleDirectionRatio[i][0] = directionIndiceU / noZero((directionIndiceU+directionIndiceV));
				angleDirectionRatio[i][1] = directionIndiceV / noZero((directionIndiceU+directionIndiceV));

				if (result< 0){
					// BendIn
					bendInFalloffData.getValueAtPosition(-result/noZero((bendInRangeData.asFloat() * paintAngleRangeArray[i])),result,&status);
					meshAnglesArray[i] = -result;
				}
				else{
					// BendOut
					bendOutFalloffData.getValueAtPosition(result/noZero((bendOutRangeData.asFloat() * paintAngleRangeArray[i])),result,&status);
					meshAnglesArray[i] = result;
				}
			}
		
			if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Calculating the angles. \n ";clo = clock(); }

			//////////////////////// Smooth the angles //////////////////////

			// Decompose the angleSmooth array into local array to work with it multithreaded

			int pasaExists = angleSmoothPaintarrayHandle.elementCount();
				for(int i=0; i<nPoints; i++) {
					angleSmoothPaintarrayHandle.jumpToArrayElement(i);
					if (pasaExists != 0) { paintAngleSmoothArray[i] = angleSmoothPaintarrayHandle.inputValue().asFloat(); } else { paintAngleSmoothArray[i] = 1; }
				}

			// Smooth it !

			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for
			#endif
			#endif
			for (int i=0; i<nPoints; i++){ meshAnglesOriginalArray[i] = meshAnglesArray[i]; }

			if (angleSmoothData.asInt() > 0){
				int i = 0; int c = 0;
				for (int s=0; s<angleSmoothData.asInt(); s++){
					#ifndef _DEBUG
					#ifdef _OPENMP
					#pragma omp parallel for private(c) 
					#endif
					#endif
					for (i=0; i<nPoints; i++){
						if ( ((float)s/noZero((float)angleSmoothData.asInt())) < paintAngleSmoothArray[i]) {
							for (c=0; c<connectedVertsMatrix[i][0]; c++){
								int index = connectedVertsMatrix[i][(c+1)];
								meshAnglesArray[i] += meshAnglesOriginalArray[index];
							}
							meshAnglesArray[i] = meshAnglesArray[i] / noZero(( connectedVertsMatrix[i][0] + 1 ));
						}
					}
					// We finished smoothing one level. Copy meshAngleArray into meshAngleOriginalArray.
					#ifndef _DEBUG
					#ifdef _OPENMP
					#pragma omp parallel for
					#endif
					#endif
					for (i=0; i<nPoints; i++){ meshAnglesOriginalArray[i] = meshAnglesArray[i]; }
				}
			}
			if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Smoothing the angles. \n ";clo = clock(); }
		}

		//############################################################################
		//################## APPLY THE ENVELOPES/MULTIPLIERS #########################
		//############################################################################

		// Decompose the weights into local array to work with it multithreaded

		float* decomposedWeightsArray = NULL;

		decomposedWeightsArray = new float[nPoints];
		int geomIndex = 0;
		for(int i=0; i<nPoints; i++) {
			decomposedWeightsArray[i] =  weightValue( data, geomIndex, i );
		}
		//////////////// apply our envelope / multipliers to the stretch & angles array values ///////////////////////
		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for(int i=0; i<nPoints; i++) {
			if ( stretchArray[i] < 0 ) {
				stretchArray[i] = stretchArray[i] * env * decomposedWeightsArray[i] * squashMultiplierData.asFloat() * tensionEnvelopeData.asFloat();
			} else {
				stretchArray[i] = stretchArray[i] * env * decomposedWeightsArray[i] * stretchMultiplierData.asFloat() * tensionEnvelopeData.asFloat();
			}
			if (meshAnglesArray[i] < 0){
				meshAnglesArray[i] = meshAnglesArray[i] * angleEnvelopeData.asFloat() * env * decomposedWeightsArray[i] * bendInData.asFloat();
			} else {
				meshAnglesArray[i] = meshAnglesArray[i] * angleEnvelopeData.asFloat() * env * decomposedWeightsArray[i] * bendOutData.asFloat();
			}
		}

		//############################################################################
		//############################## GROW / SHRINK ###############################
		//############################################################################

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Getting Grow / Shrink painted weights. \n ";clo = clock(); }

		///////////////// Grow / shrink ///////////////////////

		float* bleedArray = NULL;
		bleedArray = new float[nPoints];

		for (int currentJob = 0; currentJob < 4; currentJob++){ // Iterates 4 times. 1 for stretchGrowShrink, squashGrowShrink, bendInGrowShrink & bendOutGrowShrink

			int dontEvaluate = 0;
			if ( (currentJob == 0) && (stretchGrowShrinkData.asFloat() == 0) ) { dontEvaluate = 1; }
			if ( (currentJob == 1) && (squashGrowShrinkData.asFloat() == 0) ) { dontEvaluate = 1; }
			if ( (currentJob == 2) && (bendInGrowShrinkData.asFloat() == 0) ) { dontEvaluate = 1; }
			if ( (currentJob == 3) && (bendOutGrowShrinkData.asFloat() == 0) ) { dontEvaluate = 1; }

			float fraction;
			if (currentJob == 0) { fraction = stretchGrowShrinkData.asFloat() - (int)stretchGrowShrinkData.asFloat(); }
			if (currentJob == 1) { fraction = squashGrowShrinkData.asFloat() - (int)squashGrowShrinkData.asFloat(); }
			if (currentJob == 2) { fraction = bendInGrowShrinkData.asFloat() - (int)bendInGrowShrinkData.asFloat(); }
			if (currentJob == 3) { fraction = bendOutGrowShrinkData.asFloat() - (int)bendOutGrowShrinkData.asFloat(); }

			if ( dontEvaluate == 0 ) {

				int i, c; float neighboorValue, highest, lowest;

				float iteratorAttribute;
				if (currentJob == 0) { iteratorAttribute = stretchGrowShrinkData.asFloat(); }
				if (currentJob == 1) { iteratorAttribute = squashGrowShrinkData.asFloat(); }
				if (currentJob == 2) { iteratorAttribute = bendInGrowShrinkData.asFloat(); }
				if (currentJob == 3) { iteratorAttribute = bendOutGrowShrinkData.asFloat(); }

				for (int s=0; s<(abs((int)iteratorAttribute)+1); s++){
					#ifndef _DEBUG
					#ifdef _OPENMP
					#pragma omp parallel for private(c,i,neighboorValue, highest, lowest)
					#endif
					#endif
					for (i=0; i<nPoints; i++){

						float arrayValue;
						
						// Find the average values of tension of all neighboors points. also find the highest and lowest neighboors values
						neighboorValue = 0; highest = -999999; lowest = 999999;
						for (c=0; c<connectedVertsMatrix[i][0]; c++){
							int index = connectedVertsMatrix[i][(c+1)];
							
							if ((currentJob == 0) || (currentJob == 1)) { arrayValue = stretchArray[index]; } else { arrayValue = meshAnglesArray[index]; }
							
							neighboorValue += arrayValue;

							if (arrayValue > highest) { highest = arrayValue; }
							if (arrayValue < lowest) { lowest = arrayValue; }
						}
						if ((currentJob == 0) || (currentJob == 1)) { arrayValue = stretchArray[i]; } else { arrayValue = meshAnglesArray[i]; }

						neighboorValue /= noZero(connectedVertsMatrix[i][0]);

						float currentPaintedValue = 0;
						if (currentJob == 0) { currentPaintedValue = (float)paintStretchGrowShrinkSmoothArray[i]; }
						if (currentJob == 1) { currentPaintedValue = (float)paintSquashGrowShrinkSmoothArray[i]; }
						if (currentJob == 2) { currentPaintedValue = (float)paintBendInGrowShrinkSmoothArray[i]; }
						if (currentJob == 3) { currentPaintedValue = (float)paintBendOutGrowShrinkSmoothArray[i]; }

						// Limits
						if (currentPaintedValue > 1) { currentPaintedValue = 1; }
						if (currentPaintedValue < -1) { currentPaintedValue = -1; }


						if ( (iteratorAttribute * currentPaintedValue) > 0){ // GROW
							if ( s <= (iteratorAttribute * currentPaintedValue ) ) {
							float modifiedFraction = fraction;
							if (currentPaintedValue < 0) { modifiedFraction *= -1; }

							float highestLowest = highest + lowest;

							if ((currentJob == 0) || (currentJob == 3)) {	if (neighboorValue < 0){ highestLowest = arrayValue; }  }
							else {											if (neighboorValue > 0){ highestLowest = arrayValue; }  }

							if (s < abs((int)iteratorAttribute)){ bleedArray[i] = highestLowest; }
							else { bleedArray[i] = (arrayValue + ((highestLowest - arrayValue) * modifiedFraction)); } //fraction

							if (bleedArray[i] > highest) { bleedArray[i] = highest; }
							if (bleedArray[i] < lowest) { bleedArray[i] = lowest; }

							// No flicker here
							if (currentJob == 0) { if (bleedArray[i] < stretchArray[i]) { bleedArray[i] = stretchArray[i]; } }
							if (currentJob == 1) { if (bleedArray[i] > stretchArray[i]) { bleedArray[i] = stretchArray[i]; } }
							if (currentJob == 2) { if (bleedArray[i] > meshAnglesArray[i]) { bleedArray[i] = meshAnglesArray[i]; } }
							if (currentJob == 3) { if (bleedArray[i] < meshAnglesArray[i]) { bleedArray[i] = meshAnglesArray[i]; } }
							} else { bleedArray[i] = arrayValue; }
						}
						else { // SHRINK
							if ( -s >= (iteratorAttribute * currentPaintedValue ) ) {
							float modifiedFraction = fraction;
							if (currentPaintedValue < 0) { modifiedFraction *= -1; }
							bleedArray[i] = arrayValue;
							if ((currentJob == 0) || (currentJob == 3)) { if (neighboorValue > 0) { // stretch & squash
								if (lowest < 0) { lowest = 0; }
								if (s < abs((int)iteratorAttribute)){ bleedArray[i] = lowest; }
								if (s == abs((int)iteratorAttribute)){ bleedArray[i] = arrayValue - ((lowest - arrayValue) * modifiedFraction ); } //fraction
							} }
							else { if (neighboorValue < 0) { // blendIn & blendOut
								if (highest > 0) { highest = 0; }
								if (s < abs((int)iteratorAttribute)){ bleedArray[i] = highest; }
								if (s == abs((int)iteratorAttribute)){ bleedArray[i] = arrayValue - ((highest - arrayValue) * modifiedFraction ); } //fraction
							} }
							} else { bleedArray[i] = arrayValue; }
						}

					}
					if ((currentJob == 0) || (currentJob == 1)) {
						for (i=0; i<nPoints; i++){ stretchArray[i] = bleedArray[i]; }
					} else {
						for (i=0; i<nPoints; i++){ meshAnglesArray[i] = bleedArray[i]; }
					}
				}
			}
		}
		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Calculating the Grow / Shrink values. \n ";clo = clock(); }

		////////////////////////////// POST SMOOTH /////////////////////////

		// For tension

		if (tensionPostSmoothData.asInt() > 0){
			for (int n = 0;n<tensionPostSmoothData.asInt();n++){
				int nEdges;
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for private(nEdges)
				#endif
				#endif
				for(int i=0; i<nPoints; i++) {
					if ( (paintTensionSmoothArray[i] * tensionPostSmoothData.asInt() ) > n){
						float stretch = 0;
						for ( nEdges = 0; nEdges < connectedVertsMatrix[i][0]; nEdges++ ){
							stretch += stretchArray[connectedVertsMatrix[i][(nEdges+1)]];
						}
						tensionSmoothArray[i] = ( stretch / noZero(nEdges) );
					}
					else { tensionSmoothArray[i] = stretchArray[i]; }
				}
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for
				#endif
				#endif
				for (int e=0; e<nPoints; e++){
					stretchArray[e] = tensionSmoothArray[e];
				}
			}
		}

		// For angles
		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for (int i=0; i<nPoints; i++){ meshAnglesOriginalArray[i] = meshAnglesArray[i]; }

		if (anglePostSmoothData.asInt() > 0){
			int i = 0; int c = 0;
			for (int s=0; s<anglePostSmoothData.asInt(); s++){
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for private(c) 
				#endif
				#endif
				for (i=0; i<nPoints; i++){
					if ( ((float)s/noZero((float)anglePostSmoothData.asInt())) < paintAngleSmoothArray[i]) {
						for (c=0; c<connectedVertsMatrix[i][0]; c++){
							int index = connectedVertsMatrix[i][(c+1)];
							meshAnglesArray[i] += meshAnglesOriginalArray[index];
						}
						meshAnglesArray[i] = meshAnglesArray[i] / noZero(( connectedVertsMatrix[i][0] + 1 ));
					}
				}
				// We finished smoothing one level. Copy meshAngleArray into meshAngleOriginalArray.
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for
				#endif
				#endif
				for (i=0; i<nPoints; i++){ meshAnglesOriginalArray[i] = meshAnglesArray[i]; }
			}
		}

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Post Smoothing. \n ";clo = clock(); }

		//############################## TENSION & ANGLE FALLOFF ##############################

		float multiplier;

		// For tension ..

		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for private(multiplier)
		#endif
		#endif
		for (int i=0; i<nPoints; i++){
			
			if (stretchArray[i] < 0) { // Squash
				multiplier = noZero(squashMultiplierData.asFloat() * tensionEnvelopeData.asFloat());
				tensionFalloffData.getValueAtPosition(abs(stretchArray[i]) / multiplier,stretchArray[i],&status);
				stretchArray[i] *= -multiplier;
			}
			else { // Stretch
				multiplier = noZero(stretchMultiplierData.asFloat() * tensionEnvelopeData.asFloat());
				tensionFalloffData.getValueAtPosition(stretchArray[i] / multiplier,stretchArray[i],&status);
				stretchArray[i] *= multiplier;
			}
		}

		// For angle ..
		if (angleEnvelopeData.asFloat() > 0){
			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for private(multiplier)
			#endif
			#endif
			for (int i=0; i<nPoints; i++){

				if (meshAnglesArray[i] < 0) { // Bend In
					multiplier = noZero(bendInData.asFloat() * angleEnvelopeData.asFloat());
					angleFalloffData.getValueAtPosition(abs(meshAnglesArray[i]) / multiplier,meshAnglesArray[i],&status);
					meshAnglesArray[i] *= -multiplier;
				}
				else { // Bend Out
					multiplier = noZero(bendOutData.asFloat() * angleEnvelopeData.asFloat());
					angleFalloffData.getValueAtPosition(meshAnglesArray[i] / multiplier,meshAnglesArray[i],&status);
					meshAnglesArray[i] *= multiplier;
				}
			}
		}

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Tension / Angle Falloff. \n ";clo = clock(); }


		//############################################################################
		//############################## DEFORM VERTS ! ##############################
		//############################################################################

		#ifndef _DEBUG
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		#endif
		for(int i=0; i<nPoints; i++) {

			verts[i] = vertsRest[i]; 

			// This is to determined which blendshape should it take depending on which direction it's heading for TENSION
			float tU = tensionDirectionRatio[i][0];
			float tV = tensionDirectionRatio[i][1];
			float tmesh; float tUmesh; float tVmesh;
			MPoint tPT;

			if (tU<=.5) { tmesh = (tU/.5); } if (tU > .5) { tmesh = (tV/.5); }
			if (tU<.5) { tUmesh = 0; } else { tUmesh = (tU-.5)/.5; }
			if (tV<.5) { tVmesh = 0; } else { tVmesh = (tV-.5)/.5; }

			// This is to determined which blendshape should it take depending on which direction it's heading for ANGLES
			float aU = angleDirectionRatio[i][0];
			float aV = angleDirectionRatio[i][1];
			float amesh; float aUmesh; float aVmesh;
			MPoint aPT;

			if (aU<=.5) { amesh = (aU/.5); } if (aU > .5) { amesh = (aV/.5); }
			if (aU<.5) { aUmesh = 0; } else { aUmesh = (aU-.5)/.5; }
			if (aV<.5) { aVmesh = 0; } else { aVmesh = (aV-.5)/.5; }

			verts[i].x *= scaleFactorData.asFloat();
			verts[i].y *= scaleFactorData.asFloat();
			verts[i].z *= scaleFactorData.asFloat();

			if ( stretchArray[i] < 0 ) {
				// SQUASH
				tPT = (vertsSquash[i] * tmesh) + (vertsUSquash[i] * tUmesh) + (vertsVSquash[i] * tVmesh);
				verts[i] += ( ( (tPT - vertsRest[i])*scaleFactorData.asFloat() ) * -stretchArray[i] );
			} else {
				// STRETCH
				tPT = (vertsStretch[i] * tmesh) + (vertsUStretch[i] * tUmesh) + (vertsVStretch[i] * tVmesh);
				verts[i] += ( ( (tPT - vertsRest[i])*scaleFactorData.asFloat() ) * stretchArray[i] );
			}
			if (angleEnvelopeData.asFloat() > 0){
				if (meshAnglesArray[i] < 0){
					// BEND IN
					aPT = (vertsBendIn[i] * amesh) + (vertsUBendIn[i] * aUmesh) + (vertsVBendIn[i] * aVmesh);
					verts[i] += ((aPT - vertsRest[i])*scaleFactorData.asFloat() ) * -meshAnglesArray[i];
				} else {
					// BEND OUT
					aPT = (vertsBendOut[i] * amesh) + (vertsUBendOut[i] * aUmesh) + (vertsVBendOut[i] * aVmesh);
					verts[i] += ((aPT - vertsRest[i])*scaleFactorData.asFloat() ) * meshAnglesArray[i];
				}
			}
		}

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Deforming vertex. \n ";clo = clock(); }

		//###################################################################################
		//###################### Record vertex's relations for WRAP #########################
		//###################################################################################

		// Computes only when...
		if (wrapData.asBool() == 1){
			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for
			#endif
			#endif
			for(int i=0; i<nPoints; i++) {
				// Go through each triangle that this vertex is associated with
				for (int triIndex = 0; triIndex<vertexTrisMatrix[i][0]; triIndex++){
					// Query restMesh's vertex.
					MPoint A = vertsRest[(int)vertexTrisMatrix[i][((triIndex*6+1) + 0)]] * scaleFactorData.asFloat();
					MPoint B = vertsRest[(int)vertexTrisMatrix[i][((triIndex*6+1) + 1)]] * scaleFactorData.asFloat();
					MPoint C = vertsRest[(int)vertexTrisMatrix[i][((triIndex*6+1) + 2)]] * scaleFactorData.asFloat();

					MVector faceNormal = getNormal( A, B, C );
					MPoint pt = verts[i];

					MPoint result = getWrapValues( A, B, C, faceNormal, pt);

					vertexTrisMatrix[i][((triIndex*6+1) + 3)] = (float)result.x;
					vertexTrisMatrix[i][((triIndex*6+1) + 4)] = (float)result.y;
					vertexTrisMatrix[i][((triIndex*6+1) + 5)] = (float)result.z;

				}
			}

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Recording vertex's information for wrap. \n ";clo = clock(); }

		//###################################################################################
		//################################ Apply our WRAP ###################################
		//###################################################################################

			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for
			#endif
			#endif
			for (int i=0; i<nPoints; i++){

				// Loop trough each vertex, get their wrap relation values (U,V,N) and apply them to each point of our geometry.

				MPoint finalResult;
				finalResult.x = 0;
				finalResult.y = 0;
				finalResult.z = 0;

				for (int i2=0; i2<vertexTrisMatrix[i][0]; i2++){

					MPoint A = vertsIn[ (int)vertexTrisMatrix[i][((i2*6+1))] ];
					MPoint B = vertsIn[ (int)vertexTrisMatrix[i][((i2*6+1) + 1)] ];
					MPoint C = vertsIn[ (int)vertexTrisMatrix[i][((i2*6+1) + 1 + 1)] ];

					MVector faceNormal = getNormal( A, B, C );

					MPoint resultPoint = getWrapResult( A, B, C, faceNormal, vertexTrisMatrix[i][((i2*6+1) + 3)], vertexTrisMatrix[i][((i2*6+1) + 4)], vertexTrisMatrix[i][((i2*6+1) + 5)]);

					finalResult = ( finalResult + resultPoint );
				}

				finalResult = ( finalResult / noZero(vertexTrisMatrix[i][0]) );
				verts[i] = finalResult;
			}
		}

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Applying the wrap. \n ";clo = clock(); }

		//############################## Relax Vertex ##############################

		for (int currentJob = 0; currentJob < 4; currentJob++){

			MPointArray relaxationArray;
			relaxationArray.setLength(nPoints);

			int currentRelaxData;
			if (currentJob == 0){ currentRelaxData = stretchRelaxData.asInt(); }
			if (currentJob == 1){ currentRelaxData = squashRelaxData.asInt(); }
			if (currentJob == 2){ currentRelaxData = bendInRelaxData.asInt(); }
			if (currentJob == 3){ currentRelaxData = bendOutRelaxData.asInt(); }

			for (int n = 0;n<currentRelaxData;n++){
				int nEdges;
				int currentValue;
				float currentFraction;
				float currentTensionOrAngleValue;
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for private(nEdges,currentValue,currentFraction,currentTensionOrAngleValue)
				#endif
				#endif
				for(int i=0; i<nPoints; i++) {

					if ( (currentJob == 0) || (currentJob == 1) ) { currentTensionOrAngleValue = stretchArray[i]; }
					if ( (currentJob == 2) || (currentJob == 3) ) { currentTensionOrAngleValue = meshAnglesArray[i]; }

					int doIt = 0;
					if (currentJob == 0) { if (currentTensionOrAngleValue > 0) { doIt = 1; currentTensionOrAngleValue *= paintStretchRelaxArray[i]; } }
					if (currentJob == 1) { if (currentTensionOrAngleValue < 0) { doIt = 1; currentTensionOrAngleValue *= paintSquashRelaxArray[i]; } }
					if (currentJob == 2) { if (currentTensionOrAngleValue < 0) { doIt = 1; currentTensionOrAngleValue *= paintBendInRelaxArray[i]; } }
					if (currentJob == 3) { if (currentTensionOrAngleValue > 0) { doIt = 1; currentTensionOrAngleValue *= paintBendOutRelaxArray[i]; } }

					currentValue = (currentRelaxData * currentTensionOrAngleValue);
					relaxationArray[i] = verts[i]; // if the vertex never get sets we set it here.

					if (doIt == 1) {
						if ( n < abs(currentValue) ){ // non-fraction
							
							MPoint currentPt = verts[i];
							for ( nEdges = 0; nEdges < connectedVertsMatrix[i][0]; nEdges++ ){
								currentPt += verts[connectedVertsMatrix[i][(nEdges+1)]];
							}
							relaxationArray[i] = ( currentPt / noZero((nEdges+1)) );
						}

						if ( n == abs(currentValue) ){ // fraction !
							currentFraction = (currentRelaxData * abs(currentTensionOrAngleValue) ) - abs(currentValue);
							MPoint currentPt = verts[i];
							for ( nEdges = 0; nEdges < connectedVertsMatrix[i][0]; nEdges++ ){
								currentPt += verts[connectedVertsMatrix[i][(nEdges+1)]];
							}
							relaxationArray[i] = ( verts[i] * (1-currentFraction) ) + (( currentPt / noZero((nEdges+1)) ) * currentFraction);
						}
					}

				}
				#ifndef _DEBUG
				#ifdef _OPENMP
				#pragma omp parallel for
				#endif
				#endif
				for (int e=0; e<nPoints; e++){
					verts[e] = relaxationArray[e];
				}
				relaxationArray.clear();
			}
		}

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Relaxing the vertex. \n ";clo = clock(); }

		//############################## Draw Color Per Vertex ##############################

		if (drawColorPerVertexData.asBool()){
			MColorArray colors;
			MColorArray tensionColors;
			MColorArray angleColors;
			MIntArray vertexList;
			MColor colorValue;

			vertexList.setLength(nPoints); // Set the length of our arrays so we can use ".set" afterward because if we don't, we can't work with it multithreaded. append(), insert(), remove() are not threadsafe
			tensionColors.setLength(nPoints);
			angleColors.setLength(nPoints);

			float stretch; float angle;
			#ifndef _DEBUG
			#ifdef _OPENMP
			#pragma omp parallel for private(stretch,angle,colorValue)
			#endif
			#endif
			for(int i=0; i<nPoints; i++) {
				vertexList.set(i,i);

				// Tension Based
				if ((colorDisplayData.asShort() == 0) || (colorDisplayData.asShort() == 2)){
					if (stretchArray[i] > 0){	stretch = (0.5 + ((stretchArray[i] * tensionColorContrastData.asFloat())/2) ); } // stretch
					else {						stretch = (0.5 + ((stretchArray[i] * tensionColorContrastData.asFloat())/2) ); } // squash

					tensionColorRampData.getColorAtPosition(stretch,colorValue,&status);
					tensionColors.set(colorValue,i);
				}
				// Angle Based
				if ((colorDisplayData.asShort() == 1) || (colorDisplayData.asShort() == 2)){
					if (angleEnvelopeData.asFloat() > 0){
						if (meshAnglesArray[i]< 0){	angle = 0.5 + (( meshAnglesArray[i] * angleColorContrastData.asFloat() ) /2); } // bendIn
						else{						angle = 0.5 + (( meshAnglesArray[i] * angleColorContrastData.asFloat() ) /2); } // bendOut

						angleColorRampData.getColorAtPosition(angle,colorValue,&status);
						angleColors.set(colorValue,i);
					} else { angleColorRampData.getColorAtPosition(0.5,colorValue,&status); angleColors.set(colorValue,i); }
				}
			}
			if (colorDisplayData.asShort() == 0){ colors = tensionColors; }
			if (colorDisplayData.asShort() == 1){ colors = angleColors; }
			if (colorDisplayData.asShort() == 2){
				for(int i=0; i<nPoints; i++) {
					colors.insert((tensionColors[i] + angleColors[i])/2,i);
				}
			}

			if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Calculating the vertex's colors. \n ";clo = clock(); }

			colorMesh.setVertexColors( colors, vertexList);

			// Toggle the color display on the mesh
			MGlobal::executeCommandStringResult("for ($each in `listConnections -d 1 -s 0 " + fStretch::name() + "`){ if (`nodeType $each` == \"objectSet\") { for ($eachSet in `listConnections -d 1 -s 0 $each`){ setAttr ($eachSet + \".displayColors\") 1; } } }", false, false, NULL);
			// Force display refresh
			if (forceColorRefreshData.asBool() == 1){ MGlobal::executeCommandStringResult("dgdirty "+fStretch::name()+";", false, false, NULL); }
			colorPerVertexDisplay = 1;
		}
		else{
			// Turn off color per vertex display
			if (colorPerVertexDisplay == 1) {
				MGlobal::executeCommandStringResult("for ($each in `listConnections -d 1 -s 0 " + fStretch::name() + "`){ if (`nodeType $each` == \"objectSet\") { for ($eachSet in `listConnections -d 1 -s 0 $each`){ setAttr ($eachSet + \".displayColors\") 0; } } }", false, false, NULL);
				colorPerVertexDisplay = 0;
			}
		}
		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Drawing the vertex's colors. \n ";clo = clock(); }
		// free our arrays from memory

		delete[] (paintTensionDirectionArray);
		delete[] (paintAngleDirectionArray);
		delete[] (tensionDirectionRatio);
		delete[] (paintTensionSmoothArray);
		delete[] (paintAngleSmoothArray);
		delete[] (meshAnglesArray);
		delete[] (angleDirectionRatio);
		delete[] (meshAnglesOriginalArray);
		delete[] (decomposedWeightsArray);
		delete[] (bleedArray);

		delete[] (paintStretchGrowShrinkSmoothArray);
		delete[] (paintSquashGrowShrinkSmoothArray);
		delete[] (paintBendInGrowShrinkSmoothArray);
		delete[] (paintBendOutGrowShrinkSmoothArray);

		delete[] (paintTensionRangeArray);
		delete[] (paintAngleRangeArray);
		delete[] (paintStretchRelaxArray);
		delete[] (paintSquashRelaxArray);
		delete[] (paintBendInRelaxArray);
		delete[] (paintBendOutRelaxArray);

		if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Freeing up Arrays. \n ";clo = clock(); }
	

	iter.setAllPositions(verts);
	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Applying our deformation. \n ";clo = clock(); }

	data.setClean(plug);

	if (firstComputeDone != 1){
		firstComputeDone = 1;
	}

	if(failed) {
		return MStatus::kFailure;
	}
	if (displayComputeTimeData.asBool() == true) { cout <<(clock() - clo)<<" Finishing. \n ";clo = clock(); }
	return MStatus::kSuccess;
}

