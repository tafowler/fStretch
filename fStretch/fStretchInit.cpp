
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

///////////////////////////////////// CONSTRUCTOR / DECONSTRUCTOR / CREATOR /////////////////////////////////////

fStretch::fStretch() {
	// a fStretch node has been created !
	isMatrixCreated = 0;

	firstComputeDone = 0;

	lastNumberOfVertex = 0;
	lastNumberOfEdges = 0;
	lastNumberOfPoly = 0;

	lastWrapData = false;
	lastWrapSmoothRadius = 1;
	lastWrapItemsPerRadius = 1;
	lastWrapCentralFactor = 1;
	lastWrapOneTriPerFace = true;


	lastUseAllAvailableProcessors = false;
	lastNumberOfProcessorsToUse = 1;


	lastTensionUDirection = 1;
	lastTensionVDirection = 1;
	lastAngleUDirection = 1;
	lastAngleVDirection = 1;

	colorPerVertexDisplay = 0;

	vertsUSquash.clear();
	vertsVSquash.clear();
	vertsUStretch.clear();
	vertsVStretch.clear();
	vertsUBendIn.clear();
	vertsVBendIn.clear();
	vertsUBendOut.clear();
	vertsVBendOut.clear();

	vertsRest.clear();
	vertsSquash.clear();
	vertsStretch.clear();
	vertsBendIn.clear();
	vertsBendOut.clear();

	time(&start_time);

	meshNormalsRest.clear();

	stretchArrayExists = 0;
	tensionSmoothArrayExists = 0;
	vertexTrisMatrixExists = 0;
	connectedVertsMatrixExists = 0;
	UVDirectionMatrixExists = 0;

}
fStretch::~fStretch() {
	// class destructor (free up memory !)
	// Make sure they have been created before trying to delete them
	// delete arrays
	if (firstComputeDone == 1){
		if(stretchArrayExists == 1){ delete[] stretchArray; stretchArrayExists = 0; }
		if(tensionSmoothArrayExists == 1){ delete[] tensionSmoothArray; tensionSmoothArrayExists = 0; }

		// delete matrices
		for (int e=0; e<lastNumberOfVertex; e++){
			if (vertexTrisMatrixExists == 1) { delete[] vertexTrisMatrix[e]; }
			if (connectedVertsMatrixExists == 1){ delete[] connectedVertsMatrix[e]; }
			if (UVDirectionMatrixExists == 1){ delete[] UVDirectionMatrix[e]; }
		}
		if (vertexTrisMatrixExists == 1) { delete[] vertexTrisMatrix; vertexTrisMatrixExists = 0; }
		if (connectedVertsMatrixExists == 1){ delete[] connectedVertsMatrix; connectedVertsMatrixExists = 0; }
		if (UVDirectionMatrixExists == 1){ delete[] UVDirectionMatrix; UVDirectionMatrixExists = 0; }
	}

	// free up maya arrays

	vertsUSquash.clear();
	vertsVSquash.clear();
	vertsUStretch.clear();
	vertsVStretch.clear();
	vertsUBendIn.clear();
	vertsVBendIn.clear();
	vertsUBendOut.clear();
	vertsVBendOut.clear();

	vertsRest.clear();
	vertsSquash.clear();
	vertsStretch.clear();
	vertsBendIn.clear();
	vertsBendOut.clear();

	meshNormalsRest.clear();

}

void* fStretch::creator()
{
	return new fStretch();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////// INITIALIZE ////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MStatus fStretch::initialize()
{

	//############################## Local attribute initialization ##############################

	MStatus status;
	MFnTypedAttribute mAttr;
	MFnNumericAttribute numericAttr;

	MFnEnumAttribute enumAttr;
	MFnCompoundAttribute cAttr;

	//############################## Creating the attributes ##############################

	// Create the ramp / falloff attributes

	stretchFalloff = MRampAttribute::createCurveRamp(MString("stretchFalloff"), MString("tfa"));
	status = addAttribute(stretchFalloff); if(!status)	{ cout << "ERROR in adding curveRamp Attribute!\n";	}

	squashFalloff = MRampAttribute::createCurveRamp(MString("squashFalloff"), MString("cfa"));
	status = addAttribute(squashFalloff); if(!status)	{ cout << "ERROR in adding curveRamp Attribute!\n";	}
		
	tensionColorRamp = MRampAttribute::createColorRamp(MString("tensionColorRamp"),MString("tcrmp"));
	status = addAttribute(tensionColorRamp); if(!status)	{ cout << "ERROR in adding tensionColorRamp Attribute!\n";	}

	angleColorRamp = MRampAttribute::createColorRamp(MString("angleColorRamp"),MString("acrmp"));
	status = addAttribute(angleColorRamp); if(!status)	{ cout << "ERROR in adding angleColorRamp Attribute!\n";	}

	bendInFalloff = MRampAttribute::createCurveRamp(MString("bendInFalloff"), MString("befal"));
	status = addAttribute(bendInFalloff); if(!status)	{ cout << "ERROR in adding curveRamp Attribute!\n";	}

	bendOutFalloff = MRampAttribute::createCurveRamp(MString("bendOutFalloff"), MString("stfal"));
	status = addAttribute(bendOutFalloff); if(!status)	{ cout << "ERROR in adding curveRamp Attribute!\n";	}

	// restMesh attribute
	restMesh=mAttr.create( "restMesh", "rm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( restMesh );			MCheckStatus(status, "ERROR in addAttribute\n");


	// squashMesh attribute
	squashMesh=mAttr.create( "squashMesh", "sqm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( squashMesh );	MCheckStatus(status, "ERROR in addAttribute\n");


	// stretchMesh attribute
	stretchMesh=mAttr.create( "stretchMesh", "stm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( stretchMesh );		MCheckStatus(status, "ERROR in addAttribute\n");


	// bendInMesh attribute
	bendInMesh=mAttr.create( "bendInMesh", "bndm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( bendInMesh );		MCheckStatus(status, "ERROR in addAttribute\n");


	// bendOutMesh attribute
	bendOutMesh=mAttr.create( "bendOutMesh", "strgtm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( bendOutMesh );		MCheckStatus(status, "ERROR in addAttribute\n");

	// direction meshes

	// UsquashMesh attribute
	UsquashMesh=mAttr.create( "UsquashMesh", "usqm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( UsquashMesh );	MCheckStatus(status, "ERROR in addAttribute\n");

	// VsquashMesh attribute
	VsquashMesh=mAttr.create( "VsquashMesh", "vsqm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( VsquashMesh );	MCheckStatus(status, "ERROR in addAttribute\n");

	// UstretchMesh attribute
	UstretchMesh=mAttr.create( "UstretchMesh", "ustm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( UstretchMesh );		MCheckStatus(status, "ERROR in addAttribute\n");

	// VstretchMesh attribute
	VstretchMesh=mAttr.create( "VstretchMesh", "vstm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( VstretchMesh );		MCheckStatus(status, "ERROR in addAttribute\n");

	// UbendInMesh attribute
	UbendInMesh=mAttr.create( "UbendInMesh", "ubndm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( UbendInMesh );		MCheckStatus(status, "ERROR in addAttribute\n");

	// VbendInMesh attribute
	VbendInMesh=mAttr.create( "VbendInMesh", "vbndm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( VbendInMesh );		MCheckStatus(status, "ERROR in addAttribute\n");

	// UbendOutMesh attribute
	UbendOutMesh=mAttr.create( "UbendOutMesh", "ustrgtm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( UbendOutMesh );		MCheckStatus(status, "ERROR in addAttribute\n");

	// VbendOutMesh attribute
	VbendOutMesh=mAttr.create( "VbendOutMesh", "vstrgtm", MFnMeshData::kMesh);
	mAttr.setStorable(true);
	mAttr.setChannelBox(false);
	status = addAttribute( VbendOutMesh );		MCheckStatus(status, "ERROR in addAttribute\n");

	// Global Multiplier
	tensionEnvelope = numericAttr.create( "tensionEnvelope", "tenv",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (tensionEnvelope);		MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionSmooth Attribute
	tensionSmooth = numericAttr.create( "tensionPreSmooth", "tsm",	MFnNumericData::kInt, 0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	//numericAttr.setSoftMax(20); This line makes maya crash when the weights are painted to 1 and we save the scene and reopen it... very weird
	status = addAttribute (tensionSmooth);	MCheckStatus(status, "ERROR in addAttribute\n");

	// the reason why the default value is at 1.12345 is that when, for example, it is at 1 and the user flood the value with 1 and then paint stuff, 
	//when the first values in the array are left at 1 and we save/reopen the scene, maya do not register those value. It skips them. 
	//it will only bring them back when all values are flooded in any way. Becaue of this, maya offset the paintable attribute value array, 
	//which give ackward results. This only happen when the value that is being flooded is the same as the default value. So if we use 1.12345
	//there isn't much chance that a user is gonna flood with that value !

	// paintTensionSmooth Attribute
	paintTensionSmooth = numericAttr.create( "paintTensionSmooth", "ptsm",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintTensionSmooth);	MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionSmoothAttribute Compound attribute
	tensionSmoothAttribute = cAttr.create("tensionSmoothAttribute", "tsma");
	cAttr.addChild(paintTensionSmooth);
	cAttr.setHidden(true); 
	status = addAttribute( tensionSmoothAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionPostSmooth
	tensionPostSmooth = numericAttr.create( "tensionPostSmooth", "tsgss",	MFnNumericData::kInt, 0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	status = addAttribute (tensionPostSmooth);	MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionUDirection
	tensionUDirection = numericAttr.create( "tensionUDirection", "tud",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (tensionUDirection);		MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionVDirection
	tensionVDirection = numericAttr.create( "tensionVDirection", "tvd",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (tensionVDirection);		MCheckStatus(status, "ERROR in addAttribute\n");

	// paintTensionSmooth Attribute
	paintTensionDirection = numericAttr.create( "paintTensionDirection", "ptd",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintTensionDirection);	MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionDirectionAttribute Compound attribute
	tensionDirectionAttribute = cAttr.create("tensionDirectionAttribute", "tda");
	cAttr.addChild(paintTensionDirection);
	cAttr.setHidden(true); 
	status = addAttribute( tensionDirectionAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// paintAngleDirection Attribute
	paintAngleDirection = numericAttr.create( "paintAngleDirection", "pad",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintAngleDirection);	MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionSmoothAttribute Compound attribute
	angleDirectionAttribute = cAttr.create("angleDirectionAttribute", "ada");
	cAttr.addChild(paintAngleDirection);
	cAttr.setHidden(true); 
	status = addAttribute( angleDirectionAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// stretch Multiplier
	stretchMultiplier = numericAttr.create( "stretch", "strt",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (stretchMultiplier);	MCheckStatus(status, "ERROR in addAttribute\n");

	// stretch Range
	stretchRange = numericAttr.create( "stretchRange", "str",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0.001);
	numericAttr.setSoftMax(1);
	status = addAttribute (stretchRange);	MCheckStatus(status, "ERROR in addAttribute\n");

	// stretch grow shrink
	stretchGrowShrink = numericAttr.create( "stretchGrowShrink", "sgrshrk",	MFnNumericData::kFloat, 0.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(-10);
	numericAttr.setSoftMax(10);
	status = addAttribute (stretchGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");

	// stretchGrowShrink Attribute
	paintStretchGrowShrink = numericAttr.create( "paintStretchGrowShrink", "pstgs",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true); 
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true); 
	numericAttr.setHidden(true); 
	status = addAttribute (paintStretchGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");

	// stretchGrowShrink Compound attribute
	stretchGrowShrinkAttribute = cAttr.create("stretchGrowShrinkAttribute", "stgsa");
	cAttr.addChild(paintStretchGrowShrink);
	cAttr.setHidden(true); 
	status = addAttribute( stretchGrowShrinkAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// stretch Relax
	stretchRelax = numericAttr.create( "stretchRelax", "stre",	MFnNumericData::kInt, 0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	numericAttr.setSoftMax(20);
	status = addAttribute (stretchRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// squash Multiplier
	squashMultiplier = numericAttr.create( "squash", "sqsh",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (squashMultiplier);	MCheckStatus(status, "ERROR in addAttribute\n");

	// squash Range
	squashRange = numericAttr.create( "squashRange", "sqr",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0.001);
	numericAttr.setSoftMax(1);
	status = addAttribute (squashRange);	MCheckStatus(status, "ERROR in addAttribute\n");

	// squash grow shrink
	squashGrowShrink = numericAttr.create( "squashGrowShrink", "sqgrshrk",	MFnNumericData::kFloat, 0.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(-10);
	numericAttr.setSoftMax(10);
	status = addAttribute (squashGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");


	// paintAngleSmooth Attribute
	paintSquashGrowShrink = numericAttr.create( "paintSquashGrowShrink", "psqgs",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true); 
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true); 
	numericAttr.setHidden(true); 
	status = addAttribute (paintSquashGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angleSmoothAttribute Compound attribute
	squashGrowShrinkAttribute = cAttr.create("squashGrowShrinkAttribute", "sgsa");
	cAttr.addChild(paintSquashGrowShrink);
	cAttr.setHidden(true); 
	status = addAttribute( squashGrowShrinkAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// squash Relax
	squashRelax = numericAttr.create( "squashRelax", "sqre",	MFnNumericData::kInt, 0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	numericAttr.setSoftMax(20);
	status = addAttribute (squashRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// paintTensionRange Attribute
	paintTensionRange = numericAttr.create( "paintTensionRange", "ptrang",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintTensionRange);	MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionRange Compound attribute
	tensionRangeAttribute = cAttr.create("tensionRangeAttribute", "tranga");
	cAttr.addChild(paintTensionRange);
	cAttr.setHidden(true); 
	status = addAttribute( tensionRangeAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	/////////////// Angle based attributes //////////////////

	// angleEnvelope
	angleEnvelope = numericAttr.create( "angleEnvelope", "aenv",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (angleEnvelope);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angleSmooth
	angleSmooth = numericAttr.create( "anglePreSmooth", "asm",	MFnNumericData::kInt, 0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	//numericAttr.setSoftMax(20); this will probably make maya crash.. see the other smooth above
	status = addAttribute (angleSmooth);	MCheckStatus(status, "ERROR in addAttribute\n");

	// paintAngleSmooth Attribute
	paintAngleSmooth = numericAttr.create( "paintAngleSmooth", "pasm",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true); 
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true); 
	numericAttr.setHidden(true); 
	status = addAttribute (paintAngleSmooth);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angleSmoothAttribute Compound attribute
	angleSmoothAttribute = cAttr.create("angleSmoothAttribute", "asma");
	cAttr.addChild(paintAngleSmooth);
	cAttr.setHidden(true); 
	status = addAttribute( angleSmoothAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// anglePostSmooth
	anglePostSmooth = numericAttr.create( "anglePostSmooth", "asgss",	MFnNumericData::kInt, 0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	status = addAttribute (anglePostSmooth);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angle U
	angleUDirection = numericAttr.create( "angleUDirection", "angU",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (angleUDirection);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angle V
	angleVDirection = numericAttr.create( "angleVDirection", "angV",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (angleVDirection);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendIn
	bendIn = numericAttr.create( "bendIn", "bnd",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (bendIn);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendIn Range
	bendInRange = numericAttr.create( "bendInRange", "bndr",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0.001);
	numericAttr.setSoftMax(1);
	status = addAttribute (bendInRange);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendIn grow shrink
	bendInGrowShrink = numericAttr.create( "bendInGrowShrink", "bigrshrk",	MFnNumericData::kFloat, 0.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(-10);
	numericAttr.setSoftMax(10);
	status = addAttribute (bendInGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");

	// paintAngleSmooth Attribute
	paintBendInGrowShrink = numericAttr.create( "paintBendInGrowShrink", "pbigs",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true); 
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true); 
	numericAttr.setHidden(true); 
	status = addAttribute (paintBendInGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angleSmoothAttribute Compound attribute
	bendInGrowShrinkAttribute = cAttr.create("bendInGrowShrinkAttribute", "bigsa");
	cAttr.addChild(paintBendInGrowShrink);
	cAttr.setHidden(true); 
	status = addAttribute( bendInGrowShrinkAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// bendIn Relax
	bendInRelax = numericAttr.create( "bendInRelax", "bire",	MFnNumericData::kInt, 0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	numericAttr.setSoftMax(20);
	status = addAttribute (bendInRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendOut
	bendOut = numericAttr.create( "bendOut", "stgt",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0);
	numericAttr.setSoftMax(1);
	status = addAttribute (bendOut);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendOut Range
	bendOutRange = numericAttr.create( "bendOutRange", "strr",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0.001);
	numericAttr.setSoftMax(1);
	status = addAttribute (bendOutRange);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendOut grow shrink
	bendOutGrowShrink = numericAttr.create( "bendOutGrowShrink", "bogrshrk",	MFnNumericData::kFloat, 0.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(-10);
	numericAttr.setSoftMax(10);
	status = addAttribute (bendOutGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");

	// paintAngleSmooth Attribute
	paintBendOutGrowShrink = numericAttr.create( "paintBendOutGrowShrink", "pbogs",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true); 
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true); 
	numericAttr.setHidden(true); 
	status = addAttribute (paintBendOutGrowShrink);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angleSmoothAttribute Compound attribute
	bendOutGrowShrinkAttribute = cAttr.create("bendOutGrowShrinkAttribute", "bogsa");
	cAttr.addChild(paintBendOutGrowShrink);
	cAttr.setHidden(true); 
	status = addAttribute( bendOutGrowShrinkAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// bendOut Relax
	bendOutRelax = numericAttr.create( "bendOutRelax", "bore",	MFnNumericData::kInt, 0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	numericAttr.setSoftMax(20);
	status = addAttribute (bendOutRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// paintAngleRange Attribute
	paintAngleRange = numericAttr.create( "paintAngleRange", "parang",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintAngleRange);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angleRange Compound attribute
	angleRangeAttribute = cAttr.create("angleRangeAttribute", "aranga");
	cAttr.addChild(paintAngleRange);
	cAttr.setHidden(true); 
	status = addAttribute( angleRangeAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// counterSharpAreas
	counterSharpAreas = MRampAttribute::createCurveRamp(MString("counterSharpAreas"), MString("angcsha"));
	status = addAttribute(counterSharpAreas); if(!status)	{ cout << "ERROR in adding counterSharpAreas Attribute!\n";	}

	// tensionFalloff
	tensionFalloff = MRampAttribute::createCurveRamp(MString("tensionFalloff"), MString("tsnfalloff"));
	status = addAttribute(tensionFalloff); if(!status)	{ cout << "ERROR in adding tensionFalloff Attribute!\n";	}

	// angleFalloff
	angleFalloff = MRampAttribute::createCurveRamp(MString("angleFalloff"), MString("angfalloff"));
	status = addAttribute(angleFalloff); if(!status)	{ cout << "ERROR in adding angleFalloff Attribute!\n";	}

	/////////////////// Wrap attributes ////////////////////

	// wrap
	wrap = numericAttr.create( "wrap", "wrp",	MFnNumericData::kBoolean, 1, &status );

	numericAttr.setKeyable(true);
	status = addAttribute (wrap);	MCheckStatus(status, "ERROR in addAttribute\n");

	// wrapSmoothRadius
	wrapSmoothRadius = numericAttr.create( "smoothRadius", "sr",	MFnNumericData::kInt, 1, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(1);
	numericAttr.setSoftMax(3);
	status = addAttribute (wrapSmoothRadius);	MCheckStatus(status, "ERROR in addAttribute\n");

	// wrapOneTriPerFace
	wrapOneTriPerFace = numericAttr.create( "oneTriPerFace", "otpf",	MFnNumericData::kBoolean, 1, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	numericAttr.setMax(1);
	status = addAttribute (wrapOneTriPerFace);	MCheckStatus(status, "ERROR in addAttribute\n");

	// wrapItemsPerRadius
	wrapItemsPerRadius = numericAttr.create( "ratioPerRadius", "rpr",	MFnNumericData::kFloat, 1.0, &status );

	numericAttr.setKeyable(true);
	numericAttr.setMin(0.001);
	numericAttr.setMax(1);
	numericAttr.setHidden(true);
	status = addAttribute (wrapItemsPerRadius);	MCheckStatus(status, "ERROR in addAttribute\n");

	// wrapCentralFactor
	wrapCentralFactor = numericAttr.create( "centralFactor", "cef",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setMin(0);
	numericAttr.setMax(1);
	numericAttr.setHidden(true);
	status = addAttribute (wrapCentralFactor);	MCheckStatus(status, "ERROR in addAttribute\n");

	///////////////// Color attributes ////////////////////////

	// drawColorPerVertex Attribute
	drawColorPerVertex = numericAttr.create( "drawColorPerVertex", "dcpv", MFnNumericData::kBoolean, 0, &status );

	numericAttr.setKeyable(true);
	status = addAttribute (drawColorPerVertex);	MCheckStatus(status, "ERROR in addAttribute\n");

	// color display
	colorDisplay = enumAttr.create( "colorDisplay", "codis", 0 );
	enumAttr.setStorable(true);
	enumAttr.setKeyable(true);
	enumAttr.addField("Tension Based", 0) ;
	enumAttr.addField("Angle Based", 1) ;
	enumAttr.addField("Both", 2) ;
	status = addAttribute(colorDisplay); MCheckStatus(status, "ERROR in addAttribute\n");

	// tensionColorContrast
	tensionColorContrast = numericAttr.create( "tensionColorContrast", "tcctrst",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0.0);
	numericAttr.setSoftMax(1.0);
	status = addAttribute (tensionColorContrast);	MCheckStatus(status, "ERROR in addAttribute\n");

	// angleColorContrast
	angleColorContrast = numericAttr.create( "angleColorContrast", "acctrst",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setSoftMin(0.0);
	numericAttr.setSoftMax(1.0);

	status = addAttribute (angleColorContrast);	MCheckStatus(status, "ERROR in addAttribute\n");

	// forceColorRefresh
	forceColorRefresh = numericAttr.create( "forceColorRefresh", "fcr",	MFnNumericData::kBoolean, 1, &status );
	numericAttr.setKeyable(true);
	status = addAttribute (forceColorRefresh);	MCheckStatus(status, "ERROR in addAttribute\n");

	/////////// Paintable relax attributes //////////////////

	// paintStretchRelax Attribute
	paintStretchRelax = numericAttr.create( "paintStretchRelax", "ptstrtrel",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintStretchRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// stretchRelax Compound attribute
	stretchRelaxAttribute = cAttr.create("stretchRelaxAttribute", "strtrelatt");
	cAttr.addChild(paintStretchRelax);
	cAttr.setHidden(true); 
	status = addAttribute( stretchRelaxAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// paintSquashRelax Attribute
	paintSquashRelax = numericAttr.create( "paintSquashRelax", "ptsqrel",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintSquashRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// squashRelax Compound attribute
	squashRelaxAttribute = cAttr.create("squashRelaxAttribute", "sqrelatt");
	cAttr.addChild(paintSquashRelax);
	cAttr.setHidden(true); 
	status = addAttribute( squashRelaxAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");


	// paintBendInRelax Attribute
	paintBendInRelax = numericAttr.create( "paintBendInRelax", "ptbdinrel",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintBendInRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendInRelax Compound attribute
	bendInRelaxAttribute = cAttr.create("bendInRelaxAttribute", "bdinrelatt");
	cAttr.addChild(paintBendInRelax);
	cAttr.setHidden(true); 
	status = addAttribute( bendInRelaxAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	// paintBendOutRelax Attribute
	paintBendOutRelax = numericAttr.create( "paintBendOutRelax", "ptbdotrel",	MFnNumericData::kFloat, 1.12345 );
	numericAttr.setStorable(true);
	numericAttr.setArray(true);
	numericAttr.setUsesArrayDataBuilder(true);
	numericAttr.setHidden(true);
	status = addAttribute (paintBendOutRelax);	MCheckStatus(status, "ERROR in addAttribute\n");

	// bendOutRelax Compound attribute
	bendOutRelaxAttribute = cAttr.create("bendOutRelaxAttribute", "bdotrelatt");
	cAttr.addChild(paintBendOutRelax);
	cAttr.setHidden(true); 
	status = addAttribute( bendOutRelaxAttribute );		MCheckStatus(status, "ERROR in addAttribute\n");

	//////////////////////// Acceleration attributes ///////////////////////////

	// UseAllAvailableProcessors
	UseAllAvailableProcessors = numericAttr.create( "UseAllAvailableProcessors", "uaap",	MFnNumericData::kBoolean, 1, &status );
	numericAttr.setKeyable(true);
	status = addAttribute (UseAllAvailableProcessors);	MCheckStatus(status, "ERROR in addAttribute\n");

	// NumberOfProcessorsToUse
	NumberOfProcessorsToUse = numericAttr.create( "NumberOfProcessorsToUse", "noptu",	MFnNumericData::kInt, 1, &status );
	numericAttr.setKeyable(true);
	numericAttr.setMin(1);
	status = addAttribute (NumberOfProcessorsToUse);	MCheckStatus(status, "ERROR in addAttribute\n");

	// displayWarnings Attribute
	displayWarnings = numericAttr.create( "displayWarnings", "diswar", MFnNumericData::kBoolean, 1, &status );
	numericAttr.setKeyable(true);
	status = addAttribute (displayWarnings);	MCheckStatus(status, "ERROR in addAttribute\n");

	// displayComputeTime Attribute
	displayComputeTime = numericAttr.create( "displayComputeTime", "discpt", MFnNumericData::kBoolean, 0, &status );
	numericAttr.setKeyable(true);
	numericAttr.setHidden(true);
	status = addAttribute (displayComputeTime);	MCheckStatus(status, "ERROR in addAttribute\n");

	// scaleFactor
	scaleFactor = numericAttr.create( "scaleFactor", "sclf",	MFnNumericData::kFloat, 1.0, &status );
	numericAttr.setKeyable(true);
	status = addAttribute (scaleFactor);	MCheckStatus(status, "ERROR in addAttribute\n");


	//############################## Attributes affects ##############################


	// Attributes Affects
	status = attributeAffects( restMesh, outputGeom );				MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( squashMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( stretchMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( tensionEnvelope, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( squashMultiplier, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( stretchMultiplier, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( tensionSmooth, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( drawColorPerVertex, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( colorDisplay, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( paintTensionSmooth, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintAngleSmooth, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( paintTensionDirection, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintAngleDirection, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( squashRange, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( stretchRange, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( tensionColorContrast, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( angleColorContrast, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( wrap, outputGeom );					MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( wrapSmoothRadius, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( wrapItemsPerRadius, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( wrapCentralFactor, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( wrapOneTriPerFace, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( stretchFalloff, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( squashFalloff, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendInFalloff, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendOutFalloff, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( tensionUDirection, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( tensionVDirection, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( angleEnvelope, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( angleSmooth, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( bendIn, outputGeom );				MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendOut, outputGeom );				MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( bendInRange, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendOutRange, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( angleUDirection, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( angleVDirection, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( bendInMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendOutMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( tensionColorRamp, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( angleColorRamp, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( UsquashMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( VsquashMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( UstretchMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( VstretchMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( UbendInMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( VbendInMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( UbendOutMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( VbendOutMesh, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( UseAllAvailableProcessors, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( NumberOfProcessorsToUse, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( scaleFactor, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( stretchGrowShrink, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( squashGrowShrink, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( tensionPostSmooth, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( bendInGrowShrink, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendOutGrowShrink, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( anglePostSmooth, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( paintStretchGrowShrink, outputGeom );MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintSquashGrowShrink, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintBendInGrowShrink, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintBendOutGrowShrink, outputGeom );MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( stretchRelax, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( squashRelax, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendInRelax, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( bendOutRelax, outputGeom );			MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( paintTensionRange, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintAngleRange, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( paintStretchRelax, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintSquashRelax, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintBendInRelax, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( paintBendOutRelax, outputGeom );		MCheckStatus(status, "ERROR in attributeAffects\n");

	status = attributeAffects( counterSharpAreas, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( tensionFalloff, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");
	status = attributeAffects( angleFalloff, outputGeom );	MCheckStatus(status, "ERROR in attributeAffects\n");

	return MStatus::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////// UNINIT ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MStatus uninitializePlugin( MObject obj)
{
    MStatus result;
    MFnPlugin plugin( obj );

	result = plugin.deregisterNode( fStretch::id );

	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////// PLUGIN INIT ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MStatus initializePlugin( MObject obj )
{
    MStatus result;

    MFnPlugin plugin( obj, "Matthieu Fiorilli", "2.00", "Any");

    result = plugin.registerNode( "fStretch", fStretch::id, fStretch::creator, fStretch::initialize, MPxNode::kDeformerNode );

	MString command; MString command2; MString command3; MString command4;

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////// fStretch Node Template //////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	command = "global proc AEfStretchTemplate ( string $nodeName )\
				{\
				editorTemplate -beginScrollLayout;\
				\
				editorTemplate -beginLayout \"Deformations\" -collapse 0;\
					editorTemplate -addControl envelope;\
    				editorTemplate -beginLayout \"Tension Based\" -collapse 0;\
						editorTemplate -l \"Envelope\" -addControl tensionEnvelope;\
						editorTemplate -l \"Pre Smooth\" -addControl tensionPreSmooth;\
						editorTemplate -l \"Post Smooth\" -addControl tensionPostSmooth;\
						editorTemplate -l \"U Direction\" -addControl tensionUDirection;\
						editorTemplate -l \"V Direction\" -addControl tensionVDirection;\
						editorTemplate -as;\
						editorTemplate -l \"Stretch\" -addControl stretch;\
						editorTemplate -l \"Range\" -addControl stretchRange;\
						editorTemplate -l \"Grow / Shrink\" -addControl stretchGrowShrink;\
						editorTemplate -l \"Relax\" -addControl stretchRelax;\
						editorTemplate -as;\
						editorTemplate -l \"Squash\" -addControl squash;\
						editorTemplate -l \"Range\" -addControl squashRange;\
						editorTemplate -l \"Grow / Shrink\" -addControl squashGrowShrink;\
						editorTemplate -l \"Relax\" -addControl squashRelax;\
						AEaddRampControl ($nodeName+\".tensionFalloff\");\
						AEaddRampControl ($nodeName+\".stretchFalloff\");\
						AEaddRampControl ($nodeName+\".squashFalloff\");\
					editorTemplate -endLayout;\
					\
    				editorTemplate -beginLayout \"Angle Based\" -collapse 1;\
    					editorTemplate -l \"Envelope\" -addControl angleEnvelope;\
    					editorTemplate -l \"Pre Smooth\" -addControl anglePreSmooth;\
						editorTemplate -l \"Post Smooth\" -addControl anglePostSmooth;\
						editorTemplate -l \"U Direction\" -addControl angleUDirection;\
						editorTemplate -l \"V Direction\" -addControl angleVDirection;\
    					editorTemplate -as;\
						editorTemplate -addControl bendIn;\
						editorTemplate -l \"Range\" -addControl bendInRange;\
						editorTemplate -l \"Grow / Shrink\" -addControl bendInGrowShrink;\
						editorTemplate -l \"Relax\" -addControl bendInRelax;\
						editorTemplate -as;\
						editorTemplate -addControl bendOut;\
						editorTemplate -l \"Range\" -addControl bendOutRange;\
						editorTemplate -l \"Grow / Shrink\" -addControl bendOutGrowShrink;\
						editorTemplate -l \"Relax\" -addControl bendOutRelax;\
						AEaddRampControl ($nodeName+\".angleFalloff\");\
						AEaddRampControl ($nodeName+\".bendInFalloff\");\
						AEaddRampControl ($nodeName+\".bendOutFalloff\");\
						editorTemplate -beginLayout \"Advanced\" -collapse 1;\
							AEaddRampControl ($nodeName+\".counterSharpAreas\");\
						editorTemplate -endLayout;\
					editorTemplate -endLayout;\
			    \
				editorTemplate -endLayout;\
			    \
				editorTemplate -beginLayout \"Wrap\" -collapse 1;\
					editorTemplate -addControl wrap;\
					editorTemplate -addControl smoothRadius;\
    				editorTemplate -beginLayout \"Optimization\" -collapse 1;\
    					editorTemplate -addControl oneTriPerFace;\
/*editorTemplate -addControl ratioPerRadius;*/\
/*editorTemplate -addControl centralFactor;*/\
					editorTemplate -endLayout;\
				editorTemplate -endLayout;\
			    \
				editorTemplate -beginLayout \"Color Per Vertex\" -collapse 1;\
					editorTemplate -addControl drawColorPerVertex;\
					editorTemplate -addControl colorDisplay;\
    				editorTemplate -as;\
					editorTemplate -addControl tensionColorContrast;\
					editorTemplate -addControl angleColorContrast;\
      				editorTemplate -as;\
					editorTemplate -addControl forceColorRefresh;\
					AEaddRampControl ($nodeName+\".tensionColorRamp\");\
					editorTemplate -cl 0 ($nodeName+\".tensionColorRamp\");\
					AEaddRampControl ($nodeName+\".angleColorRamp\");\
					editorTemplate -cl 0 ($nodeName+\".angleColorRamp\");\
				editorTemplate -endLayout;\
			    \
				editorTemplate -beginLayout \"Acceleration\" -collapse 0;\
/*editorTemplate -addControl accelerationType;*/\
/*editorTemplate -beginLayout \"Multithreading\" -collapse 0;*/\
					editorTemplate -addControl UseAllAvailableProcessors;\
					editorTemplate -addControl NumberOfProcessorsToUse;\
/*editorTemplate -endLayout;*/\
				editorTemplate -endLayout;\
				\
				editorTemplate -beginLayout \"Node Behavior\" -collapse 1;\
					editorTemplate -addControl caching;\
					editorTemplate -addControl nodeState;\
					editorTemplate -addControl displayWarnings;\
					/*editorTemplate -addControl displayComputeTime;*/\
				editorTemplate -endLayout;\
				\
				editorTemplate -addExtraControls;\
				editorTemplate -endScrollLayout;\
/* Hide useless stuff*/\
			    \
				editorTemplate -s input;\
				editorTemplate -s weightList;\
				editorTemplate -s displayComputeTime;\
				editorTemplate -s map64BitIndices;\
			    \
				editorTemplate -s restMesh;\
				editorTemplate -s squashMesh;\
				editorTemplate -s stretchMesh;\
				editorTemplate -s bendInMesh;\
				editorTemplate -s bendOutMesh;\
				editorTemplate -s UsquashMesh;\
				editorTemplate -s VsquashMesh;\
				editorTemplate -s UstretchMesh;\
				editorTemplate -s VstretchMesh;\
				editorTemplate -s UbendInMesh;\
				editorTemplate -s VbendInMesh;\
				editorTemplate -s UbendOutMesh;\
				editorTemplate -s VbendOutMesh;\
			}";

	MGlobal::executeCommand(command, false, false);

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// fStretch Menu ///////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	command = "global proc fStretch_ui(){\
\
		global string $fStretch_ui;\
		if (`menu -exists $fStretch_ui`)\
		deleteUI $fStretch_ui;\
\
		string $name = \"fStretch\";\
		global string $gMainWindow;\
		$fStretch_ui = `menu -p $gMainWindow -to true -allowOptionBoxes true -l $name`;\
\
		menuItem -label \"Create fStretch\" -ann \"Apply the fStretch deformer on the selected mesh.\" -c (\"fStretch;\");\
		menuItem -divider true;\
		menuItem -subMenu true -to true -allowOptionBoxes true -label \"Connect Mesh\";\
			menuItem -label \"Stretch\" -ann \"Connect a mesh that will be used when our mesh stretch.\" -c (\"fStretch_connectMesh(\\\"stretchMesh\\\");\");\
			menuItem -label \"Squash\" -ann \"Connect a mesh that will be used when our mesh squash.\" -c (\"fStretch_connectMesh(\\\"squashMesh\\\");\");\
			menuItem -label \"Bend In\" -ann \"Connect a mesh that will be used when our mesh bend in.\" -c (\"fStretch_connectMesh(\\\"bendInMesh\\\");\");\
			menuItem -label \"Bend Out\" -ann \"Connect a mesh that will be used when our mesh bend out.\" -c (\"fStretch_connectMesh(\\\"bendOutMesh\\\");\");\
			menuItem -divider true;\
			menuItem -label \"Rest\" -ann \"Connect a mesh that represents the bindpose of our mesh.\" -c (\"fStretch_connectMesh(\\\"restMesh\\\");\");\
			menuItem -divider true;\
			menuItem -subMenu true -to true -allowOptionBoxes true -label \"Direction\";\
				menuItem -label \"U Stretch\" -ann \"Connect a mesh that will be used when our mesh stretch vertically based on it's UV.\" -c (\"fStretch_connectMesh(\\\"UstretchMesh\\\");\");\
				menuItem -label \"V Stretch\" -ann \"Connect a mesh that will be used when our mesh stretch horizontally based on it's UV.\" -c (\"fStretch_connectMesh(\\\"VstretchMesh\\\");\");\
				menuItem -divider true;\
				menuItem -label \"U Squash\" -ann \"Connect a mesh that will be used when our mesh squash vertically based on it's UV.\" -c (\"fStretch_connectMesh(\\\"UsquashMesh\\\");\");\
				menuItem -label \"V Squash\" -ann \"Connect a mesh that will be used when our mesh squash horizontally based on it's UV.\" -c (\"fStretch_connectMesh(\\\"VsquashMesh\\\");\");\
				menuItem -divider true;\
				menuItem -label \"U Bend In\" -ann \"Connect a mesh that will be used when our mesh bend in vertically based on it's UV.\" -c (\"fStretch_connectMesh(\\\"UbendInMesh\\\");\");\
				menuItem -label \"V Bend In\" -ann \"Connect a mesh that will be used when our mesh bend in horizontally based on it's UV.\" -c (\"fStretch_connectMesh(\\\"VbendInMesh\\\");\");\
				menuItem -divider true;\
				menuItem -label \"U Bend Out\" -ann \"Connect a mesh that will be used when our mesh bend out vertically based on it's UV.\" -c (\"fStretch_connectMesh(\\\"UbendOutMesh\\\");\");\
				menuItem -label \"V Bend Out\" -ann \"Connect a mesh that will be used when our mesh bend out horizontally based on it's UV.\" -c (\"fStretch_connectMesh(\\\"VbendOutMesh\\\");\");\
			setParent -menu ..;\
\
			setParent -menu ..;\
				menuItem -subMenu true -to true -allowOptionBoxes true -label \"Paint\";\
				menuItem -label \"Envelope\" -ann \"Paint the Envelope attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"weights\\\");\");\
				menuItem -divider true;\
				menuItem -label \"Tension Range\" -ann \"Paint the Tension Range attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintTensionRange\\\");\");\
				menuItem -label \"Angle Range\" -ann \"Paint the Angle Range attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintAngleRange\\\");\");\
				menuItem -divider true;\
				menuItem -label \"Tension Smooth\" -ann \"Paint the Tension Smooth attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintTensionSmooth\\\");\");\
				menuItem -label \"Angle Smooth\" -ann \"Paint the Angle Smooth attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintAngleSmooth\\\");\");\
				menuItem -divider true;\
				menuItem -label \"Tension UV Direction\" -ann \"Paint the Tension UV attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintTensionDirection\\\");\");\
				menuItem -label \"Angle UV Direction\" -ann \"Paint the Angle UV attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintAngleDirection\\\");\");\
				menuItem -divider true;\
				menuItem -label \"Stretch Grow / Shrink\" -ann \"Paint the Stretch Grow / Shrink attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintStretchGrowShrink\\\");\");\
				menuItem -label \"Squash Grow / Shrink\" -ann \"Paint the Squash Grow / Shrink attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintSquashGrowShrink\\\");\");\
				menuItem -label \"Bend In Grow / Shrink\" -ann \"Paint the Stretch Bend In / Shrink attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintBendInGrowShrink\\\");\");\
				menuItem -label \"Bend Out Grow / Shrink\" -ann \"Paint the Stretch Bend Out / Shrink attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintBendOutGrowShrink\\\");\");\
				menuItem -divider true;\
				menuItem -label \"Stretch Relax\" -ann \"Paint the Stretch Relax attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintStretchRelax\\\");\");\
				menuItem -label \"Squash Relax\" -ann \"Paint the Squash Relax attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintSquashRelax\\\");\");\
				menuItem -label \"BendIn Relax\" -ann \"Paint the BendIn Relax attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintBendInRelax\\\");\");\
				menuItem -label \"BendOut Relax\" -ann \"Paint the BendOut Relax attribute on the selected fStretch node.\" -c (\"fStretch_paint(\\\"paintBendOutRelax\\\");\");\
/*menuItem -divider true;*/\
/*menuItem -label \"Stretch Relax\" -ann \"Paint the stretchRelax attribute on the selected fStretch node.\" -c (\";\");*/\
/*menuItem -label \"Squash Relax\" -ann \"Paint the stretchRelax attribute on the selected fStretch node.\" -c (\";\");*/\
/*menuItem -label \"Bend Relax\" -ann \"Paint the bendRelax attribute on the selected fStretch node.\" -c (\";\");*/\
/*menuItem -label \"Straighten Relax\" -ann \"Paint the straightenRelax attribute on the selected fStretch node.\" -c (\";\");*/\
/*menuItem -divider true;*/\
/*menuItem -label \"Wrap Smooth Radius\" -ann \"Paint the wrapSmooth attribute on the selected fStretch node.\" -c (\";\");*/\
			setParent -menu ..;\
		setParent -menu ..;\
		menuItem -subMenu true -to true -allowOptionBoxes true -label \"Tools\";\
			menuItem -label \"Create Displacement Network...\" -ann \"Create a network that controls the displacement of the mesh based on fStretch's calculations.\" -c (\"fStretch_displacement ui;\");\
			menuItem -label \"Bake Color Per Vertex...\" -ann \"Bake the Color Per Vertex of the selected mesh into maps. The mesh needs to have an fStretch deformer on it.\" -c (\"fStretch_colorPerVertexWindow;\");\
		setParent -menu ..;\
		menuItem -divider true;\
		menuItem -label \"Help\" -ann \"Launch fStretch's online help.\" -c (\"showHelp -a \\\"http://cgaddict.com/documentation.html\\\"\");\
	}";

	MGlobal::executeCommand(command, false, false);

	/////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////// fStretch Creation /////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	command = "global proc fStretch(){\
\
/* Verify if the plug-in is loaded*/\
\
		if (!`pluginInfo -q -l \"fStretch\"`) { loadPlugin \"fStretch\"; }\
		if (!`pluginInfo -q -l \"fStretch\"`) { error(\"fStretch: plug-in is not loaded.\"); }\
\
/* Verify if the selection is a mesh*/\
		string $sel[] = `ls -sl`;\
		if ((`nodeType $sel[0]` != \"transform\") && (`nodeType $sel[0]` != \"mesh\")) { error \"fStretch: Please select a mesh to apply fStretch.\"; }\
		for ($each in $sel){\
			string $shapes[] = `listRelatives -f $each`;\
			if (`nodeType $shapes[0]` != \"mesh\") { error \"fStretch: the selected object(s) are not polygonal.\"; }\
		}\
\
/* Create fStretch on selected meshes */\
\
		for ($each in $sel){\
\
/* Create it ! */\
			select -r $each;\
			string $restMesh[] = `duplicate -rr -name ($each + \"Rest\")`;\
			string $command = (\"setAttr \" + $restMesh[0] + \".visibility 0\");\
			catchQuiet(eval($command));\
			select -r $each;\
			string $node[] = `deformer -type fStretch`; string $nodeName = $node[0];";
/*			setAttr -channelBox ";
command = command + lockNumberOfProcessorsToUseInverse;
command = command + " -keyable ";
command = command + lockNumberOfProcessorsToUseInverse;
command = command + " -l ";
command = command + lockNumberOfProcessorsToUse;
command = command + " ($node[0] + \".NumberOfProcessorsToUse\");\
			editorTemplate -dc $node[0] NumberOfProcessorsToUse ";
command = command + lockNumberOfProcessorsToUse;*/
command = command + "; select -r $restMesh[0] $each;\
			fStretch_connectMesh(\"restMesh\");\
			select -r $each;\
\
/* Initialize the paintable attributes */\
\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"weights\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintTensionSmooth\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintAngleSmooth\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintTensionDirection\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintAngleDirection\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintStretchGrowShrink\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintSquashGrowShrink\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendInGrowShrink\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendOutGrowShrink\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintTensionRange\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintAngleRange\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintStretchRelax\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintSquashRelax\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendInRelax\";\
			makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendOutRelax\";\
\
			delete $restMesh[0];\
\
			int $vertexNumber[] = `polyEvaluate -v $each`;\
			string $oneList = \"\"; string $halfList = \"\"; int $n=0;\
\
			int $amount = 0;\
\
			progressWindow\
				-title \"Creating fStretch...\"\
				-progress $amount\
				-status \"      Working hard: 0%      \"\
				-isInterruptable true;\
\
			int $break = 0;\
			for ($n=0;$n<$vertexNumber[0];$n++) {\
				$oneList = ($oneList + \" 1\");\
				$halfList = ($halfList + \" 0.5\");\
\
				if ( `progressWindow -query -isCancelled` ) {\
					$break = 1; break;\
				}\
				float $varA = $n; float $varB = $vertexNumber[0];\
				$amount = (($varA/($varB)) * 100);\
				progressWindow -edit\
					-progress $amount\
					-status (\"      Working hard... \"+$amount+\"%      \");\
			}\
			progressWindow -endProgress;\
			if ($break) {\
				delete $restMesh;\
				delete $node;\
				error(\"fStretch: cancelled.\");\
			}\
\
			string $command;\
/* Init tension smooth */\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".ptsm[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
/* Init angle smooth */\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".pasm[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
/* Init tension direction */\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".ptd[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $halfList); eval($command);\
/* Init angle direction */\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".pad[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $halfList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".pstgs[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".psqgs[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".pbigs[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".pbogs[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".ptrang[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".parang[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".ptstrtrel[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".ptsqrel[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".ptbdinrel[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
			$command = (\"setAttr -s \" + $vertexNumber[0] + \" \\\"\" + $node[0] + \".ptbdotrel[0:\" + ($vertexNumber[0]-1) + \"]\\\"\" + $oneList); eval($command);\
\
/*makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintTensionSmooth\";\
/*string $initialCtx = \"selectSuperContext\";*/\
\
/*ArtPaintAttrToolOptions;*/\
/*float $default = 1;*/\
/*string $ctx = `currentCtx`;*/\
/*float $opacity = `artAttrCtx -q -opacity $ctx`;*/\
/*float $value = `artAttrCtx -q -value $ctx`;*/\
/*artAttrCtx -e -opacity 1.0 $ctx;*/\
/*artAttrCtx -e -value $default $ctx;*/\
/*artAttrCtx -e -clear $ctx;*/\
/*artAttrCtx -e -opacity $opacity $ctx;*/\
/*artAttrCtx -e -value $value $ctx;*/\
/*setToolTo $initialCtx;*/\
\
/* add entries to the falloff attributes*/\
\
			setAttr ($nodeName+\".stretchFalloff[0].stretchFalloff_Interp\") 1;\
			setAttr ($nodeName+\".stretchFalloff[0].stretchFalloff_Position\") 1;\
			setAttr ($nodeName+\".stretchFalloff[0].stretchFalloff_FloatValue\") 1;\
			setAttr ($nodeName+\".stretchFalloff[1].stretchFalloff_Interp\") 1;\
			setAttr ($nodeName+\".stretchFalloff[1].stretchFalloff_Position\") 0;\
			setAttr ($nodeName+\".stretchFalloff[1].stretchFalloff_FloatValue\") 0;\
\
			setAttr ($nodeName+\".squashFalloff[0].squashFalloff_Interp\") 1;\
			setAttr ($nodeName+\".squashFalloff[0].squashFalloff_Position\") 1;\
			setAttr ($nodeName+\".squashFalloff[0].squashFalloff_FloatValue\") 1;\
			setAttr ($nodeName+\".squashFalloff[1].squashFalloff_Interp\") 1;\
			setAttr ($nodeName+\".squashFalloff[1].squashFalloff_Position\") 0;\
			setAttr ($nodeName+\".squashFalloff[1].squashFalloff_FloatValue\") 0;\
\
			setAttr ($nodeName+\".bendInFalloff[0].bendInFalloff_Interp\") 1;\
			setAttr ($nodeName+\".bendInFalloff[0].bendInFalloff_Position\") 1;\
			setAttr ($nodeName+\".bendInFalloff[0].bendInFalloff_FloatValue\") 1;\
			setAttr ($nodeName+\".bendInFalloff[1].bendInFalloff_Interp\") 1;\
			setAttr ($nodeName+\".bendInFalloff[1].bendInFalloff_Position\") 0;\
			setAttr ($nodeName+\".bendInFalloff[1].bendInFalloff_FloatValue\") 0;\
\
			setAttr ($nodeName+\".bendOutFalloff[0].bendOutFalloff_Interp\") 1;\
			setAttr ($nodeName+\".bendOutFalloff[0].bendOutFalloff_Position\") 1;\
			setAttr ($nodeName+\".bendOutFalloff[0].bendOutFalloff_FloatValue\") 1;\
			setAttr ($nodeName+\".bendOutFalloff[1].bendOutFalloff_Interp\") 1;\
			setAttr ($nodeName+\".bendOutFalloff[1].bendOutFalloff_Position\") 0;\
			setAttr ($nodeName+\".bendOutFalloff[1].bendOutFalloff_FloatValue\") 0;\
\
			setAttr ($nodeName+\".tensionColorRamp[0].tensionColorRamp_Color\") -type double3 1 0 0;\
			setAttr ($nodeName+\".tensionColorRamp[0].tensionColorRamp_Position\") 0;\
			setAttr ($nodeName+\".tensionColorRamp[0].tensionColorRamp_Interp\") 1;\
\
			setAttr ($nodeName+\".tensionColorRamp[1].tensionColorRamp_Color\") -type double3 1 1 0;\
			setAttr ($nodeName+\".tensionColorRamp[1].tensionColorRamp_Position\") 1;\
			setAttr ($nodeName+\".tensionColorRamp[1].tensionColorRamp_Interp\") 1;\
\
			setAttr ($nodeName+\".angleColorRamp[0].angleColorRamp_Color\") -type double3 0 0 1;\
			setAttr ($nodeName+\".angleColorRamp[0].angleColorRamp_Position\") 0;\
			setAttr ($nodeName+\".angleColorRamp[0].angleColorRamp_Interp\") 1;\
\
			setAttr ($nodeName+\".angleColorRamp[1].angleColorRamp_Color\") -type double3 .5 1 0;\
			setAttr ($nodeName+\".angleColorRamp[1].angleColorRamp_Position\") 1;\
			setAttr ($nodeName+\".angleColorRamp[1].angleColorRamp_Interp\") 1;\
\
			setAttr ($nodeName+\".counterSharpAreas[0].counterSharpAreas_Interp\") 1;\
			setAttr ($nodeName+\".counterSharpAreas[0].counterSharpAreas_Position\") 1;\
			setAttr ($nodeName+\".counterSharpAreas[0].counterSharpAreas_FloatValue\") 0;\
			setAttr ($nodeName+\".counterSharpAreas[1].counterSharpAreas_Interp\") 1;\
			setAttr ($nodeName+\".counterSharpAreas[1].counterSharpAreas_Position\") 0;\
			setAttr ($nodeName+\".counterSharpAreas[1].counterSharpAreas_FloatValue\") 0;\
\
			setAttr ($nodeName+\".tensionFalloff[0].tensionFalloff_Interp\") 2;\
			setAttr ($nodeName+\".tensionFalloff[0].tensionFalloff_Position\") 1;\
			setAttr ($nodeName+\".tensionFalloff[0].tensionFalloff_FloatValue\") 1;\
			setAttr ($nodeName+\".tensionFalloff[1].tensionFalloff_Interp\") 2;\
			setAttr ($nodeName+\".tensionFalloff[1].tensionFalloff_Position\") 0;\
			setAttr ($nodeName+\".tensionFalloff[1].tensionFalloff_FloatValue\") 0;\
\
			setAttr ($nodeName+\".angleFalloff[0].angleFalloff_Interp\") 2;\
			setAttr ($nodeName+\".angleFalloff[0].angleFalloff_Position\") 1;\
			setAttr ($nodeName+\".angleFalloff[0].angleFalloff_FloatValue\") 1;\
			setAttr ($nodeName+\".angleFalloff[1].angleFalloff_Interp\") 2;\
			setAttr ($nodeName+\".angleFalloff[1].angleFalloff_Position\") 0;\
			setAttr ($nodeName+\".angleFalloff[1].angleFalloff_FloatValue\") 0;\
		}\
	fStretch_window(\"creation\");\
	}";

	MGlobal::executeCommand(command, false, false);

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////// fStretch Connect Mesh ///////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	command = "global proc fStretch_connectMesh(string $mesh){\
					int $failed=1; int $meshes=0; string $fStretch;\
					string $sel[] = `ls -sl`;\
					if (size($sel) > 1){\
						string $history[] = `listHistory -pruneDagObjects true -interestLevel 0 $sel[1]`;\
						for ($each in $history) { if (`nodeType $each` == \"fStretch\") { $fStretch = $each; $failed=0; } } \
						for ($each in `listRelatives -f $sel[0]`) { if (`nodeType $each` == \"mesh\") { $meshes=1; } } \
						for ($each in `listRelatives -f $sel[1]`) { if (`nodeType $each` == \"mesh\") { $meshes*=2; } } \
						if (!$failed && ($meshes >= 2) ) {\
							print (\"fStretch: \" + $mesh + \" connected.\"); connectAttr -f ($sel[0] + \".outMesh\") ($fStretch + \".\" + $mesh);\
							if (`getAttr ($fStretch + \".displayWarnings\")` == 1) { if ( ($mesh == \"UstretchMesh\") || ($mesh == \"VstretchMesh\") || ($mesh == \"UsquashMesh\") || ($mesh == \"VsquashMesh\") || ($mesh == \"UbendInMesh\") || ($mesh == \"VbendInMesh\") || ($mesh == \"UbendOutMesh\") || ($mesh == \"VbendOutMesh\") ) {fStretch_window(\"connectUVDirectionMesh\"); } }\
						}\
					}\
				}";

	MGlobal::executeCommand(command, false, false);

	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////// fStretch Paint Attribute /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	command = "global proc fStretch_paint(string $fStretch_paintAttribute){\
\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"weights\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintTensionSmooth\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintAngleSmooth\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintTensionDirection\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintAngleDirection\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintStretchGrowShrink\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintSquashGrowShrink\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendInGrowShrink\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendOutGrowShrink\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintTensionRange\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintAngleRange\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintStretchRelax\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintSquashRelax\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendInRelax\";\
					makePaintable -attrType \"multiFloat\" -sm \"deformer\" \"fStretch\" \"paintBendOutRelax\";\
\
					string $sel[] = `ls -sl`;\
					if (size($sel) != 1){ error(\"fStretch: Select only one mesh to paint.\"); }\
					string $shapes[] = `listRelatives -f $sel[0]`;\
					for ($each in $shapes){\
    					$connections = `listHistory -pruneDagObjects true -interestLevel 1 ($each)`;\
    					for ($eachCon in $connections){\
    						if ( (`nodeType $eachCon`) == \"fStretch\") {\
								if (`getAttr ($eachCon + \".displayWarnings\")` == 1) { if ( ($fStretch_paintAttribute == \"paintTensionDirection\") || ($fStretch_paintAttribute == \"paintAngleDirection\") ) { fStretch_window(\"UVDirection\"); } }\
    							ArtPaintAttrToolOptions;\
    							artSetToolAndSelectAttr( \"artAttrCtx\", (\"fStretch.\" + $eachCon + \".\" + $fStretch_paintAttribute) );\
    						}\
    					}\
					}\
					\
					print (\"fStretch: painting the \" + $fStretch_paintAttribute + \" attribute.\");\
				}";

	MGlobal::executeCommand(command, false, false);


	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// fStretch Windows ////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	command ="global proc fStretch_window(string $name){\
\
/* Check out the version of maya so we use the correct windows size. */\
		string $about = `about -v`; string $mayaVersion[];\
		tokenizeList($about, $mayaVersion);\
		int $mayaYear = $mayaVersion[0];\
\
		if ($name == \"UVDirection\"){\
\
			if (fStretch_prefs(\"r\",0,0) == 0) {\
				int $height; if ($mayaYear < 2011) { $height = 185; } else { $height = 160; }\
if (($mayaYear < 2011) && (`about -li`)) { $height = 160; }\
				string $window = `window -title \"fStretch - UV Direction feature \" -iconName \"fStretch - UV Direction feature\" -rtf 1 -tlb 1 -s 1`;\
				columnLayout -adjustableColumn 1 -cal \"center\" -co \"both\" 20;\
				text -h 20 \"\\n\";\
				text \"Warning ! fStretch makes use of the mesh's UVs in order to\\ndetermine the direction. Please make sure that your\\nmesh's UVs are final before using the UV Direction feature. \\n\";\
				checkBox -label \"Do not show this warning again.\" -v 0 -onc (\"fStretch_prefs(\\\"w\\\",0,1)\") -ofc (\"fStretch_prefs(\\\"w\\\",0,0)\");\
				text -h 10 \"\\n\";\
				button -label \"Close\" -w 100 -rs 1 -h 30 -command (\"deleteUI -window \" + $window);\
				text -h 20 \"\\n\";\
				setParent ..;\
				showWindow $window;\
				window -e -wh 400 $height -s 0 $window;\
			}\
\
		}\
		if ($name == \"creation\"){\
\
			if (fStretch_prefs(\"r\",3,0) == 0) {\
				int $height; if ($mayaYear < 2011) { $height = 185; } else { $height = 160; }\
if (($mayaYear < 2011) && (`about -li`)) { $height = 160; }\
				string $window = `window -title \"fStretch - Creation \" -iconName \"fStretch - Creation\" -rtf 1 -tlb 1 -s 1`;\
				columnLayout -adjustableColumn 1 -cal \"center\" -co \"both\" 20;\
				text -h 20 \"\\n\";\
				text \"fStretch has been created. \\nPlease note that this has to be done\\n while the geometry is at its neutral pose. \\n\";\
				checkBox -label \"Do not show this warning again.\" -v 0 -onc (\"fStretch_prefs(\\\"w\\\",3,1)\") -ofc (\"fStretch_prefs(\\\"w\\\",3,0)\");\
				text -h 10 \"\\n\";\
				button -label \"Close\" -w 100 -rs 1 -h 30 -command (\"deleteUI -window \" + $window);\
				text -h 20 \"\\n\";\
				setParent ..;\
				showWindow $window;\
				window -e -wh 400 $height -s 0 $window;\
			}\
\
		}\
		if ($name == \"displacementColorRamp\"){\
			if (fStretch_prefs(\"r\",2,0) == 0) {\
				int $height; if ($mayaYear < 2011) { $height = 185; } else { $height = 160; }\
\
				string $window = `window -title \"fStretch - Create Displacement Network \" -iconName \"fStretch - Create Displacement Network\" -rtf 1 -tlb 1 -s 1`;\
				columnLayout -adjustableColumn 1 -cal \"center\" -co \"both\" 20;\
				text -h 20 \"\\n\";\
				text \"Warning ! The Tension and/or the Angle color ramp(s) \\nof the involved fStretch node will be set to grayscale\\n in order to drive the displacement maps accordingly.\\n Please do not alter the ramps' values.\\n\";\
				checkBox -label \"Do not show this warning again.\" -v 0 -onc (\"fStretch_prefs(\\\"w\\\",2,1)\") -ofc (\"fStretch_prefs(\\\"w\\\",2,0)\");\
				text -h 10 \"\\n\";\
				button -label \"Close\" -w 100 -rs 1 -h 30 -command (\"deleteUI -window \" + $window);\
				text -h 20 \"\\n\";\
				setParent ..;\
				showWindow $window;\
				window -e -wh 350 $height -s 0 $window;\
			}\
		}\
\
		if ($name == \"acceleration\"){\
\
			int $height; if ($mayaYear < 2011) { $height = 190; } else { $height = 165; }\
if (($mayaYear < 2011) && (`about -li`)) { $height = 160; }\
			string $window = `window -title \"fStretch - Acceleration feature\" -iconName \"fStretch - demo\" -rtf 1 -tlb 1 -s 1`;\
			columnLayout -adjustableColumn 1 -cal \"center\" -co \"both\" 20;\
			text -h 20 \"\\n\";\
			text \"fStretch's evaluation does not include the acceleration feature.\\nTo learn how fast can fStretch run with this feature,\\nplease visit our website.\\n\";\
			button -label \"Learn more about acceleration\" -w 100 -rs 1 -command (\"showHelp -a \\\"http://www.cgaddict.com\\\"\");\
			text -h 10 \"\\n\";\
			button -label \"Close\" -w 100 -h 30 -rs 1 -command (\"deleteUI -window \" + $window);\
			text -h 20 \"\\n\";\
			setParent ..;\
			showWindow $window;\
			window -e -wh 400 $height -s 0 $window;\
		}\
\
		if ($name == \"demoEnd\"){\
\
			int $height; if ($mayaYear < 2011) { $height = 180; } else { $height = 140; }\
if (($mayaYear < 2011) && (`about -li`)) { $height = 150; }\
			string $window = `window -title \"fStretch - Evaluation\" -iconName \"fStretch - demo\" -rtf 1 -tlb 1 -s 1`;\
			columnLayout -adjustableColumn 1 -cal \"center\" -co \"both\" 20;\
			text -h 20 \"\\n\";\
			text \"Thank you for evaluating fStretch. \\nThe demonstration period is over.\";\
			text -h 15 \"\\n\";\
			button -label \"Go to fStretch's website\" -w 100 -rs 1 -command (\"showHelp -a \\\"http://www.cgaddict.com\\\"\");\
			text -h 10 \"\\n\";\
			button -label \"Close\" -w 100 -h 30 -rs 1 -command (\"deleteUI -window \" + $window);\
			text -h 20 \"\\n\";\
			setParent ..;\
			showWindow $window;\
			window -e -wh 250 $height -s 0 $window;\
		}\
\
		if ($name == \"demo\"){\
\
			int $height; if ($mayaYear < 2011) { $height = 145; } else { $height = 125; }\
if (($mayaYear < 2011) && (`about -li`)) { $height = 160; }\
			string $window = `window -title \"fStretch - Evaluation\" -iconName \"fStretch - demo\" -rtf 1 -tlb 1 -s 1`;\
			columnLayout -adjustableColumn 1 -cal \"center\" -co \"both\" 20;\
			text -h 20 \"\\n\";\
			text \"No valid license found for fStretch. \\nThe plug-in will load in evaluation mode.\\n\\n Please take note that the acceleration\\n features are disabled in the trial mode.\\n\";\
			button -label \"Close\" -w 100 -h 30 -rs 1 -command (\"deleteUI -window \" + $window);\
			text -h 20 \"\\n\";\
			setParent ..;\
			showWindow $window;\
			window -edit -widthHeight 300 $height -s 0 $window;\
		}\
\
		if ($name == \"connectUVDirectionMesh\"){\
			if (fStretch_prefs(\"r\",1,0) == 0) {\
				int $height; if ($mayaYear < 2011) { $height = 185; } else { $height = 160; }\
if (($mayaYear < 2011) && (`about -li`)) { $height = 160; }\
				string $window = `window -title \"fStretch - UV Direction meshes \" -iconName \"fStretch\" -rtf 1 -tlb 1 -s 1`;\
				columnLayout -adjustableColumn 1 -cal \"center\" -co \"both\" 20;\
				text -h 20 \"\\n\";\
				text \"Please do not forget to update your UV Direction meshes if you change the \\nstandard meshes. For example, if you change the Stretch mesh a lot and do not\\nchange its U and V meshes accordingly, you might end up with undesired results. \\n\";\
				checkBox -label \"Do not show this warning again.\" -v 0 -onc (\"fStretch_prefs(\\\"w\\\",1,1)\") -ofc (\"fStretch_prefs(\\\"w\\\",1,0)\");\
				text -h 10 \"\\n\";\
				button -label \"Close\" -w 100 -rs 1 -h 30 -command (\"deleteUI -window \" + $window);\
				text -h 20 \"\\n\";\
				setParent ..;\
				showWindow $window;\
				window -e -wh 500 $height -s 0 $window;\
			}\
		}\
	}";

	MGlobal::executeCommand(command, false, false);


	/////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Folder Browser (old mayas) /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

command = "global proc fStretch_browse_dir( string $directory , string $type)\
{\
    global string $fStretch_browse_dir;\
    $fStretch_browse_dir=$directory + \"/\";\
}";
MGlobal::executeCommand(command, false, false);

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////// fStretch Create Displacement Network ////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

command = "global proc fStretch_displacement(string $value){\
\
    switch ($value){\
\
    case \"ui\":\
         /* fStretch displacement network window */\
\
	if (`window -exists fStretch_displacement`)\
		deleteUI fStretch_displacement;\
window -title \"fStretch - Create Displacement Network\" -mxb 0 -rtf 1 -s 0 fStretch_displacement;\
\
		columnLayout;\
\
    		text -label \"\";\
string $about = `about -v`; string $mayaVersion[];\
tokenizeList($about, $mayaVersion);\
int $mayaYear = $mayaVersion[0];\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 5 -cw5 10 100 90 225 240 -ct5 \"both\" \"both\" \"both\" \"both\" \"left\"; } else {\
		rowLayout -nc 5 -cw5 10 95 85 196 230 -ct5 \"both\" \"both\" \"both\" \"both\" \"left\"; }\
                text \" \";\
                checkBox -label \"Tension Based\" -v 1 -cc (\"fStretch_displacement tensionBased\") fStretch_disp_tensionBased;\
                checkBox -label \"Angle Based\" -v 0 -cc (\"fStretch_displacement angleBased\") fStretch_disp_angleBased;\
                checkBox -label \"Attach Pre/Post render MEL scripts\" -v 1 -cc (\"fStretch_displacement attachPrePost\") fStretch_disp_melScripts;\
                checkBox -label \"Append existing Pre/Post render MEL scripts\" -v 1 fStretch_disp_overwriteMelScripts;\
            setParent..;\
            text \" \";\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 10 200 490 -ct3 \"left\" \"both\" \"both\"; } else {\
			rowLayout -nc 3 -cw3 10 175 450 -ct3 \"left\" \"both\" \"both\"; }\
                text -label \"\";\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 45 115 35 -ct3 \"left\" \"both\" \"both\"; } else {\
				rowLayout -nc 3 -cw3 40 90 35 -ct3 \"left\" \"both\" \"both\"; }\
                    text \"Mesh :\";\
                    textField -text \"\" -cc (\"fStretch_displacement meshChange\") fStretch_disp_mesh;\
                    button -l \"Pick\" -c (\"fStretch_displacement meshPick\") fStretch_disp_meshPick;\
                setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 6 -cw6 85 115 35 105 115 35 -ct6 \"both\" \"both\" \"both\" \"right\" \"both\" \"both\"; } else {\
				rowLayout -nc 6 -cw6 85 90 35 95 90 35 -ct6 \"both\" \"both\" \"both\" \"right\" \"both\" \"both\"; }\
\
                    text \"fStretch Node :\";\
                    textField -text \"\" fStretch_disp_node;\
                    button -l \"Pick\" -c (\"fStretch_displacement nodePick\") fStretch_disp_nodePick;\
                    text \"Shading Group : \";\
                    textField -text \"\" fStretch_disp_shadingGroup;\
                    button -l \"Pick\" -c (\"fStretch_displacement shadingGroupPick\") fStretch_disp_shadingGroupPick;\
                setParent..;\
            setParent..;\
            text -label \"\";\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 2 -cw2 10 690 -ct2 \"both\" \"both\"; } else {\
			rowLayout -nc 2 -cw2 10 620 -ct2 \"both\" \"both\"; }\
            text -label \"\";\
            frameLayout -label \"Displacement Maps\" -borderStyle \"in\";\
columnLayout;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 75 555 50 -ct3 \"left\" \"both\" \"both\"; } else {\
				rowLayout -nc 3 -cw3 60 500 50 -ct3 \"left\" \"both\" \"both\"; }\
        			text -l \"  Neutral :\" fStretch_disp_neutral;\
        			textField -text \"\" fStretch_disp_neutralFile;\
        			button -l \"Browse\" -c (\"fStretch_displacement neutralBrowse\") fStretch_disp_neutralBrowse;\
    			setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 75 555 50 -ct3 \"left\" \"both\" \"both\"; } else {\
				rowLayout -nc 3 -cw3 60 500 50 -ct3 \"left\" \"both\" \"both\"; }\
        			text -l \"  Stretch :\" fStretch_disp_stretch;\
        			textField -text \"\" fStretch_disp_stretchFile;\
        			button -l \"Browse\" -c (\"fStretch_displacement stretchBrowse\") fStretch_disp_stretchBrowse;\
    			setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 75 555 50 -ct3 \"left\" \"both\" \"both\"; } else {\
				rowLayout -nc 3 -cw3 60 500 50 -ct3 \"left\" \"both\" \"both\"; }\
        			text -l \"  Squash :\" fStretch_disp_squash;\
        			textField -text \"\" fStretch_disp_squashFile;\
        			button -l \"Browse\" -c (\"fStretch_displacement squashBrowse\") fStretch_disp_squashBrowse;\
    			setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 75 555 50 -ct3 \"left\" \"both\" \"both\"; } else {\
				rowLayout -nc 3 -cw3 60 500 50 -ct3 \"left\" \"both\" \"both\"; }\
        			text -en 0 -l \"  Bend In :\" fStretch_disp_bendIn;\
        			textField -en 0 -text \"\" fStretch_disp_bendInFile;\
        			button -en 0 -l \"Browse\" -c (\"fStretch_displacement bendInBrowse\") fStretch_disp_bendInBrowse;\
    			setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 75 555 50 -ct3 \"left\" \"both\" \"both\"; } else {\
				rowLayout -nc 3 -cw3 60 500 50 -ct3 \"left\" \"both\" \"both\"; }\
        			text -en 0 -l \"  Bend Out :\" fStretch_disp_bendOut;\
        			textField -en 0 -text \"\" fStretch_disp_bendOutFile;\
        			button -en 0 -l \"Browse\" -c (\"fStretch_displacement bendOutBrowse\") fStretch_disp_bendOutBrowse;\
    			setParent..;\
setParent..;\
            setParent..;\
        setParent..;\
\
        text -label \"\";\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 2 -cw2 10 690 -ct2 \"both\" \"both\"; } else {\
		rowLayout -nc 2 -cw2 10 620 -ct2 \"both\" \"both\"; }\
            text -label \"\";\
            frameLayout -label \"Pre/Post render MEL scripts\" -borderStyle \"in\" fStretch_disp_PrePostLayout;\
columnLayout;\
                text -label \"\" -h 5;\
if (($mayaYear < 2011) && (`about -li`)) { rowColumnLayout -numberOfColumns 2 -columnWidth 1 430 -columnWidth 2 300; } else {\
                rowColumnLayout -numberOfColumns 2\
        			-columnWidth 1 340\
					-columnWidth 2 300; }\
\
if (($mayaYear < 2011) && (`about -li`)) { rowColumnLayout -numberOfColumns 2 -columnWidth 1 215 -columnWidth 2 220; } else {\
            		rowColumnLayout -numberOfColumns 2\
            			-columnWidth 1 170\
						-columnWidth 2 200; }\
                    optionMenu -label \"   Export value:  \" fStretch_disp_exportValue;\
            			menuItem -label \"Luminance\";\
            			menuItem -label \"Alpha\";\
            			menuItem -label \"RGB\";\
            			menuItem -label \"RGBA\";\
\
            		optionMenu -label \"   Image format: \" fStretch_disp_imageFormat;\
            			menuItem -label \"Gif\";\
            			menuItem -label \"SoftImage\";\
            			menuItem -label \"RLA\";\
            			menuItem -label \"TIFF\";\
            			menuItem -label \"SGI\";\
            			menuItem -label \"Alias\";\
            			menuItem -label \"IFF\";\
            			menuItem -label \"JPEG\";\
            			menuItem -label \"EPS\";\
\
            		setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 4 -cw4 75 40 85 40 -ct4 \"both\" \"both\" \"right\" \"both\"; } else {\
				rowLayout -nc 4 -cw4 75 60 65 60 -ct4 \"both\" \"both\" \"right\" \"both\"; }\
            			text -l \"Map Size X:\" fStretch_disp_mapSizeXText;\
            			intField -value 512 fStretch_disp_mapSizeX;\
            			text -l \"Map Size Y: \" fStretch_disp_mapSizeYText;\
            			intField -value 512 fStretch_disp_mapSizeY;\
        			setParent..;\
    			setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 2 -cw2 385 350 -ct2 \"both\" \"both\"; } else {\
				rowLayout -nc 2 -cw2 350 300 -ct2 \"both\" \"both\"; }\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 4 -cw4 120 75 110 75 -ct4 \"both\" \"both\" \"both\" \"both\"; } else {\
				rowLayout -nc 4 -cw4 100 75 90 75 -ct4 \"both\" \"both\" \"both\" \"both\"; }\
        			text -l \"   Tension file name:\" fStretch_disp_tensionFileNameText;\
        			textField -text \"tension\" fStretch_disp_tensionFileName;\
        			text -en 0 -l \"   Angle file name:\" fStretch_disp_angleFileNameText;\
        			textField -en 0 -text \"angle\" fStretch_disp_angleFileName;\
        		setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 55 30 205 -ct3 \"both\" \"both\" \"right\"; } else {\
				rowLayout -nc 3 -cw3 55 25 170 -ct3 \"both\" \"both\" \"right\"; }\
            		text -l \"Padding:\";\
            		intField -v 4 fStretch_disp_padding;\
            		checkBox -l \"Delete files after the render\" -v 1 fStretch_disp_deleteFiles;\
                setParent..;\
                setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 50 573 50 -ct3 \"both\" \"both\" \"both\"; } else {\
				rowLayout -nc 3 -cw3 40 513 50 -ct3 \"both\" \"both\" \"both\"; }\
        			text -l \"   Path:\" fStretch_disp_pathText;\
        			textField -text (`internalVar -userTmpDir`) fStretch_disp_filePath;\
        			button -l \"Browse\" -c (\"fStretch_displacement pathBrowse\") fStretch_disp_pathBrowse;\
        		setParent..;\
			text -label \"\" -h 5;\
            setParent..;\
setParent..;\
        setParent..;\
\
        text -label \"\";\
if (($mayaYear < 2011) && (`about -li`)) { rowColumnLayout -numberOfColumns 3 -columnWidth 1 10 -columnWidth 2 690 -columnWidth 3 5; } else {\
		rowColumnLayout -numberOfColumns 3\
			-columnWidth 1 10\
			-columnWidth 2 625\
			-columnWidth 3 5; }\
			text -label \"\";\
if (($mayaYear < 2011) && (`about -li`)) { button -w 590 -h 30 -align \"center\" -c (\"fStretch_displacement createDisp\") \"Create Displacement Network\"; } else {\
			button -w 530 -h 30 -align \"center\" -c (\"fStretch_displacement createDisp\") \"Create Displacement Network\"; }\
			text -label \"\";\
		setParent..;\
		text -label \"\" -h 10;\
\
	showWindow fStretch_displacement;\
if (($mayaYear < 2011) && (`about -li`)) { window -edit -widthHeight 710 435 -s 0 fStretch_displacement; }\
    optionMenu -e -sl 4 fStretch_disp_exportValue;\
    optionMenu -e -sl 8 fStretch_disp_imageFormat;\
	checkBox -e -v 0 fStretch_disp_melScripts;\
	fStretch_displacement attachPrePost;\
\
    break;";

command2 = "case \"shadingGroupPick\":\
        string $SG[]; clear $SG;\
        string $sel[] = `ls -sl`;\
        if ($sel[0] != \"\"){\
            if (`nodeType $sel[0]` == \"shadingEngine\") { $SG[0] = $sel[0]; }\
            else {\
              	string $shapes[] = listRelatives(\"-f\",\"-shapes\", \"-type\", \"mesh\");\
\
              	for ($shape in $shapes) {\
              	    $SG = stringArrayCatenate($SG,`listSets -type 1 -object ($shape + \".f[*]\")`);\
              	}\
            }\
            textField -e -text $SG[0] fStretch_disp_shadingGroup;\
        }\
    break;\
\
    case \"neutralBrowse\":\
		string $result[];\
string $about = `about -v`; string $mayaVersion[]; tokenizeList($about, $mayaVersion); int $mayaYear = $mayaVersion[0];\
if ($mayaYear < 2011){ $result[0] = `fileDialog -m 0`; } else {\
        $result = `fileDialog2 -okCaption \"Select File\" -fm 1 -dialogStyle 2`;\
}\
        textField -e -text $result[0] fStretch_disp_neutralFile;\
    break;\
\
    case \"stretchBrowse\":\
		string $result[];\
string $about = `about -v`; string $mayaVersion[]; tokenizeList($about, $mayaVersion); int $mayaYear = $mayaVersion[0];\
if ($mayaYear < 2011){ $result[0] = `fileDialog -m 0`; } else {\
        $result = `fileDialog2 -okCaption \"Select File\" -fm 1 -dialogStyle 2`;\
}\
        textField -e -text $result[0] fStretch_disp_stretchFile;\
    break;\
\
    case \"squashBrowse\":\
		string $result[];\
string $about = `about -v`; string $mayaVersion[]; tokenizeList($about, $mayaVersion); int $mayaYear = $mayaVersion[0];\
if ($mayaYear < 2011){ $result[0] = `fileDialog -m 0`; } else {\
        $result = `fileDialog2 -okCaption \"Select File\" -fm 1 -dialogStyle 2`;\
}\
        textField -e -text $result[0] fStretch_disp_squashFile;\
    break;\
\
    case \"bendInBrowse\":\
		string $result[];\
string $about = `about -v`; string $mayaVersion[]; tokenizeList($about, $mayaVersion); int $mayaYear = $mayaVersion[0];\
if ($mayaYear < 2011){ $result[0] = `fileDialog -m 0`; } else {\
        $result = `fileDialog2 -okCaption \"Select File\" -fm 1 -dialogStyle 2`;\
}\
        textField -e -text $result[0] fStretch_disp_bendInFile;\
    break;\
\
    case \"bendOutBrowse\":\
		string $result[];\
string $about = `about -v`; string $mayaVersion[]; tokenizeList($about, $mayaVersion); int $mayaYear = $mayaVersion[0];\
if ($mayaYear < 2011){ $result[0] = `fileDialog -m 0`; } else {\
        $result = `fileDialog2 -okCaption \"Select File\" -fm 1 -dialogStyle 2`;\
}\
        textField -e -text $result[0] fStretch_disp_bendOutFile;\
    break;\
\
    case \"pathBrowse\":\
		string $result[];\
string $about = `about -v`; string $mayaVersion[]; tokenizeList($about, $mayaVersion); int $mayaYear = $mayaVersion[0];\
if ($mayaYear < 2011){\
global string $fStretch_browse_dir;\
fileBrowser \"fStretch_browse_dir\" \"Choose Directory\" \"directory\" 4; $result[0] = $fStretch_browse_dir;\
textField -e -text ($result[0]) fStretch_disp_filePath;\
} else {\
        string $result[] = `fileDialog2 -okCaption \"Select Folder\" -fm 3 -dialogStyle 2`;\
        textField -e -text ($result[0] + \"/\") fStretch_disp_filePath;\
}\
    break;\
\
    case \"tensionBased\":\
        int $v = `checkBox -q -v fStretch_disp_tensionBased`;\
        text -e -en $v fStretch_disp_stretch;\
        textField -e -en $v fStretch_disp_stretchFile;\
        button -e -en $v fStretch_disp_stretchBrowse;\
        text -e -en $v fStretch_disp_squash;\
        textField -e -en $v fStretch_disp_squashFile;\
        button -e -en $v fStretch_disp_squashBrowse;\
        text -e -en $v fStretch_disp_tensionFileNameText;\
        textField -e -en $v fStretch_disp_tensionFileName;\
        if (!`checkBox -q -v fStretch_disp_tensionBased` && !`checkBox -q -v fStretch_disp_angleBased`){\
            checkBox -e -v 1 fStretch_disp_angleBased; fStretch_displacement angleBased;\
        }\
    break;\
\
    case \"angleBased\":\
        int $v = `checkBox -q -v fStretch_disp_angleBased`;\
        text -e -en $v fStretch_disp_bendIn;\
        textField -e -en $v fStretch_disp_bendInFile;\
        button -e -en $v fStretch_disp_bendInBrowse;\
        text -e -en $v fStretch_disp_bendOut;\
        textField -e -en $v fStretch_disp_bendOutFile;\
        button -e -en $v fStretch_disp_bendOutBrowse;\
        text -e -en $v fStretch_disp_angleFileNameText;\
        textField -e -en $v fStretch_disp_angleFileName;\
        if (!`checkBox -q -v fStretch_disp_angleBased` && !`checkBox -q -v fStretch_disp_tensionBased`){\
            checkBox -e -v 1 fStretch_disp_tensionBased; fStretch_displacement tensionBased;\
        }\
    break;\
\
    case \"nodePick\":\
        string $sel[] = `ls -sl`;\
        if ($sel[0] != \"\"){\
            string $history[] = `listHistory -pruneDagObjects true -interestLevel 0 $sel[0]`;\
            for ($each in $history){\
                if (`nodeType $each` == \"fStretch\"){\
                    textField -e -text $each fStretch_disp_node;\
                }\
            }\
        }\
    break;\
\
    case \"meshPick\":\
        string $sel[] = `ls -sl`;\
        if ($sel[0] != \"\"){\
            textField -e -text `ls -sl` fStretch_disp_mesh;\
            fStretch_displacement meshChange;\
            fStretch_displacement nodePick;\
            fStretch_displacement shadingGroupPick;\
        }\
    break;\
\
    case \"meshChange\":\
        textField -e -text (`textField -q -text fStretch_disp_mesh` + \"_tension\") fStretch_disp_tensionFileName;\
        textField -e -text (`textField -q -text fStretch_disp_mesh` + \"_angle\") fStretch_disp_angleFileName;\
    break;\
\
    case \"attachPrePost\":\
        checkBox -e -en `checkBox -q -v fStretch_disp_melScripts` fStretch_disp_overwriteMelScripts;\
        frameLayout -e -en `checkBox -q -v fStretch_disp_melScripts` fStretch_disp_PrePostLayout;\
    break;\
\
    case \"createDisp\":\
\
        int $incompleteWindow = 0;\
\
        if (`checkBox -q -v fStretch_disp_tensionBased`){\
            if (`textField -q -text fStretch_disp_stretchFile` == \"\"){ $incompleteWindow = 1; }\
            if (`textField -q -text fStretch_disp_squashFile` == \"\"){ $incompleteWindow = 1; }\
            if (`checkBox -q -v fStretch_disp_melScripts`){ if (`textField -q -text fStretch_disp_tensionFileName` == \"\"){ $incompleteWindow = 1; } }\
        }\
        if (`checkBox -q -v fStretch_disp_angleBased`){\
            if (`textField -q -text fStretch_disp_bendInFile` == \"\"){ $incompleteWindow = 1; }\
            if (`textField -q -text fStretch_disp_bendOutFile` == \"\"){ $incompleteWindow = 1; }\
            if (`checkBox -q -v fStretch_disp_melScripts`){ if (`textField -q -text fStretch_disp_angleFileName` == \"\"){ $incompleteWindow = 1; } }\
        }\
        if (`textField -q -text fStretch_disp_mesh` == \"\"){ $incompleteWindow = 1; }\
        if (`textField -q -text fStretch_disp_node` == \"\"){ $incompleteWindow = 1; }\
        if (`textField -q -text fStretch_disp_shadingGroup` == \"\"){ $incompleteWindow = 1; }\
        if (`textField -q -text fStretch_disp_neutralFile` == \"\"){ $incompleteWindow = 1; }\
\
        if (`checkBox -q -v fStretch_disp_melScripts`){\
            if (`intField -q -v fStretch_disp_mapSizeX` <= 0){ $incompleteWindow = 1; }\
            if (`intField -q -v fStretch_disp_mapSizeY` <= 0){ $incompleteWindow = 1; }\
            if (`intField -q -v fStretch_disp_padding` <= 0){ $incompleteWindow = 1; }\
            if (`textField -q -text fStretch_disp_filePath` == \"\"){ $incompleteWindow = 1; }\
        }\
\
        if ($incompleteWindow){\
            confirmDialog -t \"fStretch - Incomplete fields\" -m \"Please make sure to fill all the fields needed.      \" -ma \"center\" -b \"Ok\";\
            error(\"\\nfStretch: Please make sure to fill all the needed fields.\");\
        }";

command3 = "string $name = `textField -q -text fStretch_disp_node`;\
        string $tensionFile;\
        string $neutralDispFile;\
        string $stretchDispFile;\
        string $squashDispFile;\
        string $bendInDispFile;\
        string $bendOutDispFile;\
        string $angleFile;\
\
        string $stretchRemap;\
        string $squashRemap;\
        string $bendInRemap;\
        string $bendOutRemap;\
\
        string $stretchContrast;\
        string $squashContrast;\
        string $bendInContrast;\
        string $bendOutContrast;\
\
string $blendColors[]; clear $blendColors;\
\
        string $plusMinusAverage1;\
        string $plusMinusAverage2;\
\
        string $displacementNode;\
\
        string $misc[]; clear $misc;\
\
if ((`checkBox -q -v fStretch_disp_tensionBased`) && (`checkBox -q -v fStretch_disp_angleBased`)){\
            /* TENSION & ANGLE DISPLACEMENT */\
\
            /* Create file nodes */\
            int $i;\
            for ($i=0; $i<=6; $i++){\
                string $fileName;\
                if ($i == 0){ $fileName = \"tension\"; }\
                if ($i == 1){ $fileName = \"neutralDisp\"; }\
                if ($i == 2){ $fileName = \"stretchDisp\"; }\
                if ($i == 3){ $fileName = \"squashDisp\"; }\
                if ($i == 4){ $fileName = \"bendInDisp\"; }\
                if ($i == 5){ $fileName = \"bendOutDisp\"; }\
                if ($i == 6){ $fileName = \"angle\"; }\
                string $file = `shadingNode -asTexture file -name ($name+\"_\"+$fileName+\"_file\")`;\
                string $place2dTexture = `shadingNode -asUtility place2dTexture -name ($name+\"_\"+$fileName+\"_place2dTexture\")`;\
                $misc[size($misc)] = $place2dTexture;\
                connectAttr -f ($place2dTexture+\".coverage\") ($file+\".coverage\");connectAttr -f ($place2dTexture+\".translateFrame\") ($file+\".translateFrame\");\
                connectAttr -f ($place2dTexture+\".rotateFrame\") ($file+\".rotateFrame\");connectAttr -f ($place2dTexture+\".mirrorU\") ($file+\".mirrorU\");\
                connectAttr -f ($place2dTexture+\".mirrorV\") ($file+\".mirrorV\");connectAttr -f ($place2dTexture+\".stagger\") ($file+\".stagger\");\
                connectAttr -f ($place2dTexture+\".wrapU\") ($file+\".wrapU\");connectAttr -f ($place2dTexture+\".wrapV\") ($file+\".wrapV\");\
                connectAttr -f ($place2dTexture+\".repeatUV\") ($file+\".repeatUV\");connectAttr -f ($place2dTexture+\".offset\") ($file+\".offset\");\
                connectAttr -f ($place2dTexture+\".rotateUV\") ($file+\".rotateUV\");connectAttr -f ($place2dTexture+\".noiseUV\") ($file+\".noiseUV\");\
                connectAttr -f ($place2dTexture+\".vertexUvOne\") ($file+\".vertexUvOne\");connectAttr -f ($place2dTexture+\".vertexUvTwo\") ($file+\".vertexUvTwo\");\
                connectAttr -f ($place2dTexture+\".vertexUvThree\") ($file+\".vertexUvThree\");connectAttr -f ($place2dTexture+\".vertexCameraOne\") ($file+\".vertexCameraOne\");\
                connectAttr ($place2dTexture+\".outUV\") ($file+\".uv\");connectAttr ($place2dTexture+\".outUvFilterSize\") ($file+\".uvFilterSize\");\
                if ($i == 0){ $tensionFile = $file; }\
                if ($i == 1){ $neutralDispFile = $file; }\
                if ($i == 2){ $stretchDispFile = $file; }\
                if ($i == 3){ $squashDispFile = $file; }\
                if ($i == 4){ $bendInDispFile = $file; }\
                if ($i == 5){ $bendOutDispFile = $file; }\
                if ($i == 6){ $angleFile = $file; }\
            }\
\
            /* Set the files path */\
\
            setAttr -type \"string\" ($tensionFile + \".fileTextureName\") \"\";\
            setAttr -type \"string\" ($neutralDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_neutralFile`;\
            setAttr -type \"string\" ($stretchDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_stretchFile`;\
            setAttr -type \"string\" ($squashDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_squashFile`;\
            setAttr -type \"string\" ($bendInDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_bendInFile`;\
            setAttr -type \"string\" ($bendOutDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_bendOutFile`;\
            setAttr -type \"string\" ($angleFile + \".fileTextureName\") \"\";\
\
            /* Create remapColor nodes */\
            $stretchRemap = `shadingNode -name ($name + \"_stretch_remap\") -asUtility remapColor`;\
            $squashRemap = `shadingNode -name ($name + \"_squash_remap\") -asUtility remapColor`;\
            $bendInRemap = `shadingNode -name ($name + \"_bendIn_remap\") -asUtility remapColor`;\
            $bendOutRemap = `shadingNode -name ($name + \"_bendOut_remap\") -asUtility remapColor`;\
\
            /* Create contrast nodes */\
            $stretchContrast = `shadingNode -name ($name + \"_stretch_contrast\") -asUtility contrast`;\
            $squashContrast = `shadingNode -name ($name + \"_squash_contrast\") -asUtility contrast`;\
            $bendInContrast = `shadingNode -name ($name + \"_bendIn_contrast\") -asUtility contrast`;\
            $bendOutContrast = `shadingNode -name ($name + \"_bendOut_contrast\") -asUtility contrast`;\
\
            /* Create blendColors nodes */\
            for ($i=0;$i<7;$i++){\
                $blendColors[$i] = `shadingNode -name ($name + \"_blendColors\" + $i) -asUtility blendColors`;\
            }\
\
            /* Create plusMinusAverage nodes */\
            $plusMinusAverage1 = `shadingNode -name ($name + \"_plusMinusAverage1\") -asUtility plusMinusAverage`;\
            $plusMinusAverage2 = `shadingNode -name ($name + \"_plusMinusAverage2\") -asUtility plusMinusAverage`;\
\
            /* Connect the nodes */\
\
            connectAttr -f ($tensionFile + \".outColor\") ($stretchRemap + \".color\");\
            connectAttr -f ($tensionFile + \".outColor\") ($squashRemap + \".color\");\
\
            connectAttr -f ($angleFile + \".outColor\") ($bendInRemap + \".color\");\
            connectAttr -f ($angleFile + \".outColor\") ($bendOutRemap + \".color\");\
\
            connectAttr -f ($stretchRemap + \".outColorR\") ($stretchContrast + \".valueX\");\
            connectAttr -f ($stretchRemap + \".outColorR\") ($stretchContrast + \".valueY\");\
            connectAttr -f ($stretchRemap + \".outColorR\") ($stretchContrast + \".valueZ\");\
            connectAttr -f ($squashRemap + \".outColorR\") ($squashContrast + \".valueX\");\
            connectAttr -f ($squashRemap + \".outColorR\") ($squashContrast + \".valueY\");\
            connectAttr -f ($squashRemap + \".outColorR\") ($squashContrast + \".valueZ\");\
            connectAttr -f ($bendInRemap + \".outColorR\") ($bendInContrast + \".valueX\");\
            connectAttr -f ($bendInRemap + \".outColorR\") ($bendInContrast + \".valueY\");\
            connectAttr -f ($bendInRemap + \".outColorR\") ($bendInContrast + \".valueZ\");\
            connectAttr -f ($bendOutRemap + \".outColorR\") ($bendOutContrast + \".valueX\");\
            connectAttr -f ($bendOutRemap + \".outColorR\") ($bendOutContrast + \".valueY\");\
            connectAttr -f ($bendOutRemap + \".outColorR\") ($bendOutContrast + \".valueZ\");\
\
            connectAttr -f ($squashContrast + \".outValueX\") ($blendColors[1] + \".blender\");\
            connectAttr -f ($squashDispFile + \".outColor\") ($blendColors[0] + \".color1\");\
            setAttr ($blendColors[0] + \".color2\") -type double3 0.5 0.5 0.5 ;\
\
            connectAttr -f ($blendColors[0] + \".output\") ($blendColors[1] + \".color2\");\
            connectAttr -f ($stretchDispFile + \".outColor\") ($blendColors[1] + \".color1\");\
            connectAttr -f ($stretchContrast + \".outValueX\") ($blendColors[0] + \".blender\");\
\
            connectAttr -f ($blendColors[1] + \".output\") ($blendColors[2] + \".color2\");\
            connectAttr -f ($bendOutDispFile + \".outColor\") ($blendColors[2] + \".color1\");\
            connectAttr -f ($bendInContrast + \".outValueX\") ($blendColors[3] + \".blender\");\
\
            connectAttr -f ($blendColors[2] + \".output\") ($blendColors[3] + \".color2\");\
            connectAttr -f ($bendInDispFile + \".outColor\") ($blendColors[3] + \".color1\");\
            connectAttr -f ($bendOutContrast + \".outValueX\") ($blendColors[2] + \".blender\");\
\
            connectAttr -f ($blendColors[3] + \".output\") ($blendColors[4] + \".color2\");\
            connectAttr -f ($blendColors[1] + \".output\") ($blendColors[4] + \".color1\");\
\
            connectAttr -f ($stretchContrast + \".outValueX\") ($plusMinusAverage1 + \".input1D[1]\");\
            connectAttr -f ($squashContrast + \".outValueX\") ($plusMinusAverage1 + \".input1D[0]\");\
\
            connectAttr -f ($plusMinusAverage1 + \".output1D\") ($blendColors[4] + \".blender\");\
\
            connectAttr -f ($blendColors[4] + \".output\") ($blendColors[5] + \".color2\");\
            connectAttr -f ($blendColors[3] + \".output\") ($blendColors[5] + \".color1\");\
            setAttr ($blendColors[5] + \".blender\") 0.5;\
\
            connectAttr -f ($blendColors[5] + \".output\") ($blendColors[6] + \".color1\");\
            connectAttr -f ($neutralDispFile + \".outColor\") ($blendColors[6] + \".color2\");\
\
            connectAttr -f ($stretchContrast + \".outValueX\") ($plusMinusAverage2 + \".input1D[1]\");\
            connectAttr -f ($squashContrast + \".outValueX\") ($plusMinusAverage2 + \".input1D[0]\");\
            connectAttr -f ($bendInContrast + \".outValueX\") ($plusMinusAverage2 + \".input1D[3]\");\
            connectAttr -f ($bendOutContrast + \".outValueX\") ($plusMinusAverage2 + \".input1D[2]\");\
\
            connectAttr -f ($plusMinusAverage2 + \".output1D\") ($blendColors[6] + \".blender\");\
\
            setAttr ($bendOutRemap+\".inputMin\") 0.5;\
            setAttr ($bendInRemap+\".inputMin\") 0.5;\
            setAttr ($bendInRemap+\".inputMax\") 0;\
\
            setAttr ($squashRemap+\".inputMin\") 0.5;\
            setAttr ($stretchRemap+\".inputMin\") 0.5;\
            setAttr ($stretchRemap+\".inputMax\") 0;\
\
            /* Create displacement node */\
\
            $displacementNode = `shadingNode -name ($name + \"_displacementShader\") -asShader displacementShader`;\
            connectAttr -f ($blendColors[6] + \".outputR\") ($displacementNode + \".displacement\");\
            connectAttr -f ($displacementNode + \".displacement\") (`textField -q -text fStretch_disp_shadingGroup` + \".displacementShader\");\
\
            /* Create Container */\
\
            select -r $tensionFile $neutralDispFile $stretchDispFile $squashDispFile $bendInDispFile $bendOutDispFile $angleFile $stretchRemap $squashRemap $bendInRemap $bendOutRemap $stretchContrast\
            $squashContrast $bendInContrast $bendOutContrast $blendColors $plusMinusAverage1 $plusMinusAverage2 $misc;\
\
            container -name ($name + \"_displacementNetwork_asset\") -includeShapes -includeTransform -force -addNode `ls -sl`;\
        }\
        else if (`checkBox -q -v fStretch_disp_tensionBased`) {\
            /* TENSION DISPLACEMENT */\
\
            /* Create file nodes */\
            int $i;\
            for ($i=0; $i<=3; $i++){\
                string $fileName;\
                if ($i == 0){ $fileName = \"tension\"; }\
                if ($i == 1){ $fileName = \"neutralDisp\"; }\
                if ($i == 2){ $fileName = \"stretchDisp\"; }\
                if ($i == 3){ $fileName = \"squashDisp\"; }\
                string $file = `shadingNode -asTexture file -name ($name+\"_\"+$fileName+\"_file\")`;\
                string $place2dTexture = `shadingNode -asUtility place2dTexture -name ($name+\"_\"+$fileName+\"_place2dTexture\")`;\
                $misc[size($misc)] = $place2dTexture;\
                connectAttr -f ($place2dTexture+\".coverage\") ($file+\".coverage\");connectAttr -f ($place2dTexture+\".translateFrame\") ($file+\".translateFrame\");\
                connectAttr -f ($place2dTexture+\".rotateFrame\") ($file+\".rotateFrame\");connectAttr -f ($place2dTexture+\".mirrorU\") ($file+\".mirrorU\");\
                connectAttr -f ($place2dTexture+\".mirrorV\") ($file+\".mirrorV\");connectAttr -f ($place2dTexture+\".stagger\") ($file+\".stagger\");\
                connectAttr -f ($place2dTexture+\".wrapU\") ($file+\".wrapU\");connectAttr -f ($place2dTexture+\".wrapV\") ($file+\".wrapV\");\
                connectAttr -f ($place2dTexture+\".repeatUV\") ($file+\".repeatUV\");connectAttr -f ($place2dTexture+\".offset\") ($file+\".offset\");\
                connectAttr -f ($place2dTexture+\".rotateUV\") ($file+\".rotateUV\");connectAttr -f ($place2dTexture+\".noiseUV\") ($file+\".noiseUV\");\
                connectAttr -f ($place2dTexture+\".vertexUvOne\") ($file+\".vertexUvOne\");connectAttr -f ($place2dTexture+\".vertexUvTwo\") ($file+\".vertexUvTwo\");\
                connectAttr -f ($place2dTexture+\".vertexUvThree\") ($file+\".vertexUvThree\");connectAttr -f ($place2dTexture+\".vertexCameraOne\") ($file+\".vertexCameraOne\");\
                connectAttr ($place2dTexture+\".outUV\") ($file+\".uv\");connectAttr ($place2dTexture+\".outUvFilterSize\") ($file+\".uvFilterSize\");\
                if ($i == 0){ $tensionFile = $file; }\
                if ($i == 1){ $neutralDispFile = $file; }\
                if ($i == 2){ $stretchDispFile = $file; }\
                if ($i == 3){ $squashDispFile = $file; }\
            }\
\
            /* Set the files path */\
\
            setAttr -type \"string\" ($tensionFile + \".fileTextureName\") \"\";\
            setAttr -type \"string\" ($neutralDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_neutralFile`;\
            setAttr -type \"string\" ($stretchDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_stretchFile`;\
            setAttr -type \"string\" ($squashDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_squashFile`;\
\
            /* Create remapColor nodes */\
            $stretchRemap = `shadingNode -name ($name + \"_stretch_remap\") -asUtility remapColor`;\
            $squashRemap = `shadingNode -name ($name + \"_squash_remap\") -asUtility remapColor`;\
\
            /* Create contrast nodes */\
            $stretchContrast = `shadingNode -name ($name + \"_stretch_contrast\") -asUtility contrast`;\
            $squashContrast = `shadingNode -name ($name + \"_squash_contrast\") -asUtility contrast`;\
\
            /* Create blendColors nodes */\
            for ($i=0;$i<2;$i++){\
                $blendColors[$i] = `shadingNode -name ($name + \"_blendColors\" + $i) -asUtility blendColors`;\
            }\
\
            /* Connect the nodes */\
\
            connectAttr -f ($tensionFile + \".outColor\") ($stretchRemap + \".color\");\
            connectAttr -f ($tensionFile + \".outColor\") ($squashRemap + \".color\");\
\
            connectAttr -f ($stretchRemap + \".outColorR\") ($stretchContrast + \".valueX\");\
            connectAttr -f ($stretchRemap + \".outColorR\") ($stretchContrast + \".valueY\");\
            connectAttr -f ($stretchRemap + \".outColorR\") ($stretchContrast + \".valueZ\");\
            connectAttr -f ($squashRemap + \".outColorR\") ($squashContrast + \".valueX\");\
            connectAttr -f ($squashRemap + \".outColorR\") ($squashContrast + \".valueY\");\
            connectAttr -f ($squashRemap + \".outColorR\") ($squashContrast + \".valueZ\");\
\
            connectAttr -f ($squashContrast + \".outValueX\") ($blendColors[1] + \".blender\");\
            connectAttr -f ($squashDispFile + \".outColor\") ($blendColors[0] + \".color1\");\
            setAttr ($blendColors[0] + \".color2\") -type double3 0.5 0.5 0.5 ;\
\
            connectAttr -f ($blendColors[0] + \".output\") ($blendColors[1] + \".color2\");\
            connectAttr -f ($stretchDispFile + \".outColor\") ($blendColors[1] + \".color1\");\
            connectAttr -f ($stretchContrast + \".outValueX\") ($blendColors[0] + \".blender\");\
\
            connectAttr -f ($neutralDispFile + \".outColor\") ($blendColors[0] + \".color2\");\
\
            setAttr ($squashRemap+\".inputMin\") 0.5;\
            setAttr ($stretchRemap+\".inputMin\") 0.5;\
            setAttr ($stretchRemap+\".inputMax\") 0;\
\
            /* Create displacement node */\
\
            $displacementNode = `shadingNode -name ($name + \"_displacementShader\") -asShader displacementShader`;\
            connectAttr -f ($blendColors[1] + \".outputR\") ($displacementNode + \".displacement\");\
            connectAttr -f ($displacementNode + \".displacement\") (`textField -q -text fStretch_disp_shadingGroup` + \".displacementShader\");\
\
            /* Create Container */\
\
            select -r $tensionFile $neutralDispFile $stretchDispFile $squashDispFile $stretchRemap $squashRemap $stretchContrast\
            $squashContrast $blendColors $misc;\
\
            container -name ($name + \"_displacementNetwork_asset\") -includeShapes -includeTransform -force -addNode `ls -sl`;\
        }";

command4 = "else if (`checkBox -q -v fStretch_disp_angleBased`) {\
            /* ANGLE DISPLACEMENT */\
\
            /* Create file nodes */\
            int $i;\
            for ($i=0; $i<=3; $i++){\
                string $fileName;\
                if ($i == 0){ $fileName = \"angle\"; }\
                if ($i == 1){ $fileName = \"neutralDisp\"; }\
                if ($i == 2){ $fileName = \"bendInDisp\"; }\
                if ($i == 3){ $fileName = \"bendOutDisp\"; }\
                string $file = `shadingNode -asTexture file -name ($name+\"_\"+$fileName+\"_file\")`;\
                string $place2dTexture = `shadingNode -asUtility place2dTexture -name ($name+\"_\"+$fileName+\"_place2dTexture\")`;\
                $misc[size($misc)] = $place2dTexture;\
                connectAttr -f ($place2dTexture+\".coverage\") ($file+\".coverage\");connectAttr -f ($place2dTexture+\".translateFrame\") ($file+\".translateFrame\");\
                connectAttr -f ($place2dTexture+\".rotateFrame\") ($file+\".rotateFrame\");connectAttr -f ($place2dTexture+\".mirrorU\") ($file+\".mirrorU\");\
                connectAttr -f ($place2dTexture+\".mirrorV\") ($file+\".mirrorV\");connectAttr -f ($place2dTexture+\".stagger\") ($file+\".stagger\");\
                connectAttr -f ($place2dTexture+\".wrapU\") ($file+\".wrapU\");connectAttr -f ($place2dTexture+\".wrapV\") ($file+\".wrapV\");\
                connectAttr -f ($place2dTexture+\".repeatUV\") ($file+\".repeatUV\");connectAttr -f ($place2dTexture+\".offset\") ($file+\".offset\");\
                connectAttr -f ($place2dTexture+\".rotateUV\") ($file+\".rotateUV\");connectAttr -f ($place2dTexture+\".noiseUV\") ($file+\".noiseUV\");\
                connectAttr -f ($place2dTexture+\".vertexUvOne\") ($file+\".vertexUvOne\");connectAttr -f ($place2dTexture+\".vertexUvTwo\") ($file+\".vertexUvTwo\");\
                connectAttr -f ($place2dTexture+\".vertexUvThree\") ($file+\".vertexUvThree\");connectAttr -f ($place2dTexture+\".vertexCameraOne\") ($file+\".vertexCameraOne\");\
                connectAttr ($place2dTexture+\".outUV\") ($file+\".uv\");connectAttr ($place2dTexture+\".outUvFilterSize\") ($file+\".uvFilterSize\");\
                if ($i == 0){ $angleFile = $file; }\
                if ($i == 1){ $neutralDispFile = $file; }\
                if ($i == 2){ $bendInDispFile = $file; }\
                if ($i == 3){ $bendOutDispFile = $file; }\
            }\
\
            /* Set the files path */\
\
            setAttr -type \"string\" ($angleFile + \".fileTextureName\") \"\";\
            setAttr -type \"string\" ($neutralDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_neutralFile`;\
            setAttr -type \"string\" ($bendInDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_bendInFile`;\
            setAttr -type \"string\" ($bendOutDispFile + \".fileTextureName\") `textField -q -text fStretch_disp_bendOutFile`;\
\
            /* Create remapColor nodes */\
            $bendInRemap = `shadingNode -name ($name + \"_bendIn_remap\") -asUtility remapColor`;\
            $bendOutRemap = `shadingNode -name ($name + \"_bendOut_remap\") -asUtility remapColor`;\
\
            /* Create contrast nodes */\
            $bendInContrast = `shadingNode -name ($name + \"_bendIn_contrast\") -asUtility contrast`;\
            $bendOutContrast = `shadingNode -name ($name + \"_bendOut_contrast\") -asUtility contrast`;\
\
            /* Create blendColors nodes */\
            for ($i=0;$i<2;$i++){\
                $blendColors[$i] = `shadingNode -name ($name + \"_blendColors\" + $i) -asUtility blendColors`;\
            }\
\
            /* Connect the nodes */\
\
            connectAttr -f ($angleFile + \".outColor\") ($bendInRemap + \".color\");\
            connectAttr -f ($angleFile + \".outColor\") ($bendOutRemap + \".color\");\
\
            connectAttr -f ($bendInRemap + \".outColorR\") ($bendInContrast + \".valueX\");\
            connectAttr -f ($bendInRemap + \".outColorR\") ($bendInContrast + \".valueY\");\
            connectAttr -f ($bendInRemap + \".outColorR\") ($bendInContrast + \".valueZ\");\
            connectAttr -f ($bendOutRemap + \".outColorR\") ($bendOutContrast + \".valueX\");\
            connectAttr -f ($bendOutRemap + \".outColorR\") ($bendOutContrast + \".valueY\");\
            connectAttr -f ($bendOutRemap + \".outColorR\") ($bendOutContrast + \".valueZ\");\
\
            connectAttr -f ($bendOutContrast + \".outValueX\") ($blendColors[1] + \".blender\");\
            connectAttr -f ($bendOutDispFile + \".outColor\") ($blendColors[1] + \".color1\");\
            setAttr ($blendColors[0] + \".color2\") -type double3 0.5 0.5 0.5 ;\
\
            connectAttr -f ($blendColors[0] + \".output\") ($blendColors[1] + \".color2\");\
            connectAttr -f ($bendInDispFile + \".outColor\") ($blendColors[0] + \".color1\");\
            connectAttr -f ($bendInContrast + \".outValueX\") ($blendColors[0] + \".blender\");\
\
            connectAttr -f ($neutralDispFile + \".outColor\") ($blendColors[0] + \".color2\");\
\
            setAttr ($bendOutRemap+\".inputMin\") 0.5;\
            setAttr ($bendInRemap+\".inputMin\") 0.5;\
            setAttr ($bendInRemap+\".inputMax\") 0;\
\
            /* Create displacement node */\
\
            $displacementNode = `shadingNode -name ($name + \"_displacementShader\") -asShader displacementShader`;\
            connectAttr -f ($blendColors[1] + \".outputR\") ($displacementNode + \".displacement\");\
            connectAttr -f ($displacementNode + \".displacement\") (`textField -q -text fStretch_disp_shadingGroup` + \".displacementShader\");\
\
            /* Create Container */\
\
            select -r $angleFile $neutralDispFile $bendInDispFile $bendOutDispFile $bendInRemap $bendOutRemap $bendInContrast\
            $bendOutContrast $blendColors $misc;\
\
            container -name ($name + \"_displacementNetwork_asset\") -includeShapes -includeTransform -force -addNode `ls -sl`;\
        }\
\
        /* Create pre/post mel scripts */\
\
        if (`checkBox -q -v fStretch_disp_melScripts`){\
\
            string $mode;\
            if (`checkBox -q -v fStretch_disp_tensionBased` && `checkBox -q -v fStretch_disp_angleBased`) { $mode = \"both\"; }\
            else if (`checkBox -q -v fStretch_disp_tensionBased`) { $mode = \"tension\"; }\
            else if (`checkBox -q -v fStretch_disp_angleBased`) { $mode = \"angle\"; }\
\
            string $mesh = `textField -q -text fStretch_disp_mesh`;\
            string $fStretchNode = `textField -q -text fStretch_disp_node`;\
            string $exportValue = `optionMenu -q -v fStretch_disp_exportValue`;\
            string $imageFormat = `optionMenu -q -v fStretch_disp_imageFormat`;\
            int $mapSizeX = `intField -q -value fStretch_disp_mapSizeX`;\
            int $mapSizeY = `intField -q -value fStretch_disp_mapSizeY`;\
            string $tensionName = `textField -q -text fStretch_disp_tensionFileName`;\
            string $angleName = `textField -q -text fStretch_disp_angleFileName`;\
            string $path = `textField -q -text fStretch_disp_filePath`;\
            int $padding = `intField -q -v fStretch_disp_padding`;\
\
            if (`checkBox -q -v fStretch_disp_overwriteMelScripts`) {\
                setAttr -type \"string\" defaultRenderGlobals.preMel (\"fStretch_displacement_preRenderMel(\\\"\"+$mode+\"\\\", \\\"\"+$mesh+\"\\\", \\\"\"+$fStretchNode+\"\\\", \\\"\"+$exportValue+\"\\\", \\\"\"+$imageFormat+\"\\\", \\\"\"+$mapSizeX+\"\\\", \\\"\"+$mapSizeY+\"\\\", \\\"\"+$tensionName+\"\\\", \\\"\"+$angleName+\"\\\", \\\"\"+$path+\"\\\", \\\"\"+$tensionFile+\"\\\", \\\"\"+$angleFile+\"\\\", \\\"\"+$padding+\"\\\"); \" + `getAttr defaultRenderGlobals.preMel`);\
                if (`checkBox -q -v fStretch_disp_deleteFiles`){ setAttr -type \"string\" defaultRenderGlobals.postMel (\"fStretch_displacement_postRenderMel(\\\"\"+$mode+\"\\\", \\\"\"+$imageFormat+\"\\\", \\\"\"+$tensionName+\"\\\", \\\"\"+$angleName+\"\\\", \\\"\"+$path+\"\\\", \\\"\"+$tensionFile+\"\\\", \\\"\"+$angleFile+\"\\\", \\\"\"+$padding+\"\\\"); \" + `getAttr defaultRenderGlobals.postMel`); }\
            } else {\
                setAttr -type \"string\" defaultRenderGlobals.preMel (\"fStretch_displacement_preRenderMel(\\\"\"+$mode+\"\\\", \\\"\"+$mesh+\"\\\", \\\"\"+$fStretchNode+\"\\\", \\\"\"+$exportValue+\"\\\", \\\"\"+$imageFormat+\"\\\", \\\"\"+$mapSizeX+\"\\\", \\\"\"+$mapSizeY+\"\\\", \\\"\"+$tensionName+\"\\\", \\\"\"+$angleName+\"\\\", \\\"\"+$path+\"\\\", \\\"\"+$tensionFile+\"\\\", \\\"\"+$angleFile+\"\\\", \\\"\"+$padding+\"\\\"); \");\
                if (`checkBox -q -v fStretch_disp_deleteFiles`){ setAttr -type \"string\" defaultRenderGlobals.postMel (\"fStretch_displacement_postRenderMel(\\\"\"+$mode+\"\\\", \\\"\"+$imageFormat+\"\\\", \\\"\"+$tensionName+\"\\\", \\\"\"+$angleName+\"\\\", \\\"\"+$path+\"\\\", \\\"\"+$tensionFile+\"\\\", \\\"\"+$angleFile+\"\\\", \\\"\"+$padding+\"\\\"); \"); }\
            }\
\
            /* gray scale warning */\
\
            fStretch_window displacementColorRamp;\
\
            /* gray scale tension ramp */\
\
            if (`checkBox -q -v fStretch_disp_tensionBased`) {\
                for ($i=20; $i>=0; $i--){\
                    catchQuiet (`removeMultiInstance -break true ($fStretchNode + \".tensionColorRamp[\" + $i + \"]\")`);\
                    refreshAE;\
                }\
                setAttr ($fStretchNode + \".tensionColorRamp[0].tensionColorRamp_Position\") 1;\
                setAttr ($fStretchNode + \".tensionColorRamp[0].tensionColorRamp_Color\") -type double3 1 1 1 ;\
				setAttr ($fStretchNode + \".tensionColorRamp[0].tensionColorRamp_Interp\") 3;\
                setAttr ($fStretchNode + \".tensionColorRamp[1].tensionColorRamp_Position\") 0;\
                setAttr ($fStretchNode + \".tensionColorRamp[1].tensionColorRamp_Color\") -type double3 0 0 0 ;\
				setAttr ($fStretchNode + \".tensionColorRamp[1].tensionColorRamp_Interp\") 3;\
            }\
\
            /* gray scale angle ramp */\
\
            if (`checkBox -q -v fStretch_disp_angleBased`) {\
                for ($i=20; $i>=0; $i--){\
                    catchQuiet (`removeMultiInstance -break true ($fStretchNode + \".angleColorRamp[\" + $i + \"]\")`);\
                    refreshAE;\
                }\
                setAttr ($fStretchNode + \".angleColorRamp[0].angleColorRamp_Position\") 1;\
                setAttr ($fStretchNode + \".angleColorRamp[0].angleColorRamp_Color\") -type double3 1 1 1 ;\
				setAttr ($fStretchNode + \".angleColorRamp[0].angleColorRamp_Interp\") 3;\
                setAttr ($fStretchNode + \".angleColorRamp[1].angleColorRamp_Position\") 0;\
                setAttr ($fStretchNode + \".angleColorRamp[1].angleColorRamp_Color\") -type double3 0 0 0 ;\
				setAttr ($fStretchNode + \".angleColorRamp[1].angleColorRamp_Interp\") 3;\
            }\
        }\
\
    break;\
\
    default:\
\
    break;\
    }\
}\
\
/*######################### Pre/Post render MEL script proc ##########################*/\
\
global proc fStretch_displacement_preRenderMel(string $mode, string $mesh, string $fStretchNode,string $exportValue,string $imageFormat,int $mapSizeX, int $mapSizeY, string $tensionName, string $angleName, string $path, string $tensionFileNode, string $angleFileNode, int $padding){\
\
	if (!`about -batch`) {\
		string $oldSel[] = `ls -sl`;\
	\
		int $oldDrawCPVValue = `getAttr ($fStretchNode + \".drawColorPerVertex\")`;\
		int $oldColorDisplayValue= `getAttr ($fStretchNode + \".colorDisplay\")`;\
	\
		select -r $mesh;\
	\
		/* Padding */\
		string $cTime = `currentTime -q`;\
		string $buffer[];\
		$numTokens = `tokenize $cTime \".\" $buffer`;\
	\
		if (($padding - `size($buffer[0])`) >= 0) { $padding = ($padding - `size($buffer[0])`); } else { $padding = 0; }\
		int $p; string $paddingZeros = \"\";\
		for ($p=0; $p<$padding; $p++){\
			$paddingZeros = ($paddingZeros + \"0\");\
		}\
	\
		/* Tension Based */\
		if ( ($mode == \"tension\") || ($mode == \"both\") ){\
	\
			setAttr ($fStretchNode + \".drawColorPerVertex\") 1;\
			setAttr ($fStretchNode + \".colorDisplay\") 0;\
	\
			PaintVertexColorToolOptions;\
			artSetToolAndSelectAttr( \"artAttrCtx\", \"mesh.\" + $mesh +\".vertexColorRGB\" );\
			artExportAttrMapCB (\"artAttrPaintVertexCtx -efm \" + $exportValue + \" -eft \" + $imageFormat + \" -fsx \" + $mapSizeX + \" -fsy \" + $mapSizeY) ($path + $tensionName + \".\" + $paddingZeros + `currentTime -q` + \".\" + $imageFormat) \"map\";\
			setAttr -type \"string\" ($tensionFileNode + \".fileTextureName\") ($path + $tensionName + \".\" + $paddingZeros + `currentTime -q` + \".\" + $imageFormat);\
			/*dgdirty $tensionFileNode;*/\
		}\
	\
		/* Angle Based */\
		if ( ($mode == \"angle\") || ($mode == \"both\") ){\
	\
			setAttr ($fStretchNode + \".drawColorPerVertex\") 1;\
			setAttr ($fStretchNode + \".colorDisplay\") 1;\
	\
			PaintVertexColorToolOptions;\
			artSetToolAndSelectAttr( \"artAttrCtx\", \"mesh.\" + $mesh +\".vertexColorRGB\" );\
			artExportAttrMapCB (\"artAttrPaintVertexCtx -efm \" + $exportValue + \" -eft \" + $imageFormat + \" -fsx \" + $mapSizeX + \" -fsy \" + $mapSizeY) ($path + $angleName + \".\" + $paddingZeros + `currentTime -q` + \".\" + $imageFormat) \"map\";\
			setAttr -type \"string\" ($angleFileNode + \".fileTextureName\") ($path + $angleName + \".\" + $paddingZeros + `currentTime -q` + \".\" + $imageFormat);\
			/*dgdirty $angleFileNode;*/\
		}\
	\
		setAttr ($fStretchNode + \".colorDisplay\") $oldColorDisplayValue;\
		setAttr ($fStretchNode + \".drawColorPerVertex\") $oldDrawCPVValue;\
		setToolTo nurbsSelect;\
	\
		select -r $oldSel;\
	}\
}\
\
global proc fStretch_displacement_postRenderMel(string $mode, string $imageFormat, string $tensionName, string $angleName, string $path, string $tensionFileNode, string $angleFileNode, int $padding){\
	if (!`about -batch`) {\
		/* Padding */\
		string $cTime = `currentTime -q`;\
		string $buffer[];\
		$numTokens = `tokenize $cTime \".\" $buffer`;\
	\
		if (($padding - `size($buffer[0])`) >= 0) { $padding = ($padding - `size($buffer[0])`); } else { $padding = 0; }\
		int $p; string $paddingZeros = \"\";\
		for ($p=0; $p<$padding; $p++){\
			$paddingZeros = ($paddingZeros + \"0\");\
		}\
	\
		/* Delete files */\
	\
		if ( ($mode == \"tension\") || ($mode == \"both\") ){\
			setAttr -type \"string\" ($tensionFileNode + \".fileTextureName\") \"\";\
			sysFile -delete ($path + $tensionName + \".\" + $paddingZeros + `currentTime -q` + \".\" + $imageFormat);\
		}\
		if ( ($mode == \"angle\") || ($mode == \"both\") ){\
			setAttr -type \"string\" ($angleFileNode + \".fileTextureName\") \"\";\
			sysFile -delete ($path + $angleName + \".\" + $paddingZeros + `currentTime -q` + \".\" + $imageFormat);\
		}\
	}\
}";
	MGlobal::executeCommand(command+command2+command3+command4, false, false);

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// fStretch bakeColorPerVertex /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	command ="/* proc to bake color per vertex*/\
\
	global proc fStretch_bakeColorPerVertex(string $mesh,string $exportValue,string $imageFormat,int $mapSizeX, int $mapSizeY, float $startFrame, float $endFrame, string $name, string $path, int $forceRefresh){\
\
		string $nameSub = $name; string $meshSub = $mesh;\
\
		while (`substitute \":\" $nameSub \"_\"` != $nameSub){$nameSub = `substitute \":\" $nameSub \"_\"`; }\
		while (`substitute \":\" $meshSub \"_\"` != $meshSub){$meshSub = `substitute \":\" $meshSub \"_\"`; }\
\
		select -r $mesh;\
		string $history[] = `listHistory -pruneDagObjects true -interestLevel 0 $mesh`;\
		$findIt = 0; string $fStretch;\
		for ($each in $history){\
			if (`nodeType $each` == \"fStretch\") { $findIt = 1; $fStretch = $each; }\
		}\
		int $oldAttr = `getAttr ($fStretch + \".drawColorPerVertex\")`;\
		if ($findIt == 1) { setAttr ($fStretch + \".drawColorPerVertex\") 1; }\
\
		PaintVertexColorToolOptions;\
\
\
		int $amount = 0;\
\
		progressWindow\
			-title \"Baking...\"\
			-progress $amount\
			-status \"Working hard: 0%\"\
			-isInterruptable true;\
\
		int $f;\
		for($f=$startFrame;$f<=$endFrame;$f++)\
		{\
			string $fString = $f;\
			int $paddingNo = (4-size($fString));\
			string $padding = \"\"; int $i=0;\
			for ($i=0;$i<$paddingNo;$i++){\
				$padding = ($padding + \"0\");\
			}\
\
			currentTime $f;\
\
			/* Check if the dialog has been cancelled */\
			if ( `progressWindow -query -isCancelled` ) break;\
\
			$amount = ((($f - $startFrame) / ($endFrame-$startFrame)) * 100.0);\
\
			progressWindow -edit\
				-progress $amount\
				-status (\"Working hard: \"+$amount+\"%\");\
\
			select -r $mesh;\
			if ($forceRefresh) { dgdirty -a; }\
			artSetToolAndSelectAttr( \"artAttrCtx\", \"mesh.\" + $mesh +\".vertexColorRGB\" );\
			artExportAttrMapCB (\"artAttrPaintVertexCtx -efm \" + $exportValue + \" -eft \" + $imageFormat + \" -fsx \" + $mapSizeX + \" -fsy \" + $mapSizeY) ($path + $meshSub + \"_\" + $nameSub + \".\" + $padding + $f + \".\" + $imageFormat) \"map\";\
		}\
\
		progressWindow -endProgress;\
\
		setAttr ($fStretch + \".drawColorPerVertex\") $oldAttr;\
	}\
\
\
	/* proc for the Window */\
\
	global proc fStretch_colorPerVertexWindow(){\
		string $about = `about -v`; string $mayaVersion[];\
		tokenizeList($about, $mayaVersion);\
		int $mayaYear = $mayaVersion[0];\
	    \
		int $height; if ($mayaYear < 2011) { $height = 225; } else { $height = 205; }\
\
		if (`window -exists fStretch_bakeColorPerVertex`)\
			deleteUI fStretch_bakeColorPerVertex;\
			window -title \"fStretch - Bake Color Per Vertex\" -widthHeight 200 $height -s 0 -mxb 0 fStretch_bakeColorPerVertex;\
\
			columnLayout;\
			text -label \"\";\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -numberOfColumns 2 -columnWidth2 205 175 -adjustableColumn 2 -columnAlign  1 \"left\" -columnAttach 1 \"both\"  0 -columnAttach 2 \"both\"  0; } else {\
			rowLayout -numberOfColumns 2\
				-columnWidth2 200 175\
				-adjustableColumn 2\
				-columnAlign  1 \"left\"\
				-columnAttach 1 \"both\"     0\
				-columnAttach 2 \"both\"  0; }\
if (($mayaYear < 2011) && (`about -li`)) { optionMenu -label \" Export value:  \" fStretch_exportValue; } else {\
			optionMenu -label \"   Export value:  \" fStretch_exportValue; }\
				menuItem -label \"Luminance\";\
				menuItem -label \"Alpha\";\
				menuItem -label \"RGB\";\
				menuItem -label \"RGBA\";\
\
			optionMenu -label \"   Image format: \" fStretch_imageFormat;\
				menuItem -label \"Gif\";\
				menuItem -label \"SoftImage\";\
				menuItem -label \"RLA\";\
				menuItem -label \"TIFF\";\
				menuItem -label \"SGI\";\
				menuItem -label \"Alias\";\
				menuItem -label \"IFF\";\
				menuItem -label \"JPEG\";\
				menuItem -label \"EPS\";\
\
			setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 4 -cw4 120 85 120 81 -ct4 \"both\" \"both\" \"right\" \"both\" -columnAlign  1 \"left\"; } else {\
			rowLayout -nc 4 -cw4 80 85 115 81 -ct4 \"both\" \"both\" \"right\" \"both\" -columnAlign  1 \"left\"; }\
				text \"   Map Size X:    \";\
				intField -value 512 fStretch_mapSizeX;\
				text \"      Map Size Y:\";\
				intField -value 512 fStretch_mapSizeY;\
				setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 4 -cw4 120 85 120 81 -ct4 \"both\" \"both\" \"right\" \"both\" -columnAlign  1 \"left\"; } else {\
			rowLayout -nc 4 -cw4 80 85 115 81 -ct4 \"both\" \"both\" \"right\" \"both\" -columnAlign  1 \"left\"; }\
				text \"   Start Frame:\";\
				intField -value 1 fStretch_startFrame;\
				text \" End Frame:\";\
				intField -value 1 fStretch_endFrame;\
				setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 2 -cw2 80 325 -ct2 \"both\" \"both\" -columnAlign  1 \"left\"; } else {\
			rowLayout -nc 2 -cw2 80 285 -ct2 \"both\" \"both\" -columnAlign  1 \"left\"; }\
				text \"   Name:\";\
				textField -text \"tension\" fStretch_name;\
				setParent..;\
if (($mayaYear < 2011) && (`about -li`)) { rowLayout -nc 3 -cw3 80 275 50 -ct3 \"both\" \"both\" \"both\" -columnAlign  1 \"left\"; } else {\
			rowLayout -nc 3 -cw3 80 230 50 -ct3 \"both\" \"both\" \"both\" -columnAlign  1 \"left\"; }\
				text \"   Path:\";\
				textField -text (`workspace -q -dir`) fStretch_path;\
				button -c (\"global string $fStretch_browse_dir; fileBrowser \\\"fStretch_browse_dir\\\" \\\"Choose Directory\\\" \\\"directory\\\" 4; textField -e -text ($fStretch_browse_dir) fStretch_path;\") \"Browse\";\
				setParent..;\
\
			rowLayout -nc 2 -cw2 80 230 -ct2 \"both\" \"both\";\
				text \" \";\
				checkBox -label \"Force refresh\" fStretch_forceRefresh;\
				setParent..;\
\
       			text -l \" \" -h 10;\
if (($mayaYear < 2011) && (`about -li`)) { rowColumnLayout -numberOfColumns 3 -columnWidth 1 10 -columnWidth 2 395 -columnWidth 3 10; } else {\
			rowColumnLayout -numberOfColumns 3\
				-columnWidth 1 10\
				-columnWidth 2 355\
				-columnWidth 3 10; }\
				text -label \"\";\
if (($mayaYear < 2011) && (`about -li`)) { button -w 395 -h 30 -c \"fStretch_bakeColorPerVertex_window()\" -align \"center\" \"Bake Color Per Vertex\"; } else {\
				button -w 355 -h 30 -c \"fStretch_bakeColorPerVertex_window()\" -align \"center\" \"Bake Color Per Vertex\"; }\
				text -label \" \";\
			setParent..;\
			text -h 10 \"\\n\";\
\
		showWindow fStretch_bakeColorPerVertex;\
\
		global string $fStretch_bakeColorPerVertex_presets[];\
\
		int $mapSizeX = $fStretch_bakeColorPerVertex_presets[2];\
		int $mapSizeY = $fStretch_bakeColorPerVertex_presets[3];\
		int $startFrame = $fStretch_bakeColorPerVertex_presets[4];\
		int $endFrame = $fStretch_bakeColorPerVertex_presets[5];\
		int $forceRefresh = $fStretch_bakeColorPerVertex_presets[8];\
\
		optionMenu -e -value \"RGBA\" fStretch_exportValue;\
		optionMenu -e -value \"JPEG\" fStretch_imageFormat;\
		intField -e -value `playbackOptions -q -min` fStretch_startFrame;\
		intField -e -value `playbackOptions -q -max` fStretch_endFrame;\
\
		if ($fStretch_bakeColorPerVertex_presets[0] != \"\") { optionMenu -e -value $fStretch_bakeColorPerVertex_presets[0] fStretch_exportValue; }\
		if ($fStretch_bakeColorPerVertex_presets[1] != \"\") { optionMenu -e -value $fStretch_bakeColorPerVertex_presets[1] fStretch_imageFormat; }\
		if ($fStretch_bakeColorPerVertex_presets[2] != \"\") { intField -e -value $mapSizeX fStretch_mapSizeX; }\
		if ($fStretch_bakeColorPerVertex_presets[3] != \"\") { intField -e -value $mapSizeY fStretch_mapSizeY; }\
		if ($fStretch_bakeColorPerVertex_presets[4] != \"\") { intField -e -value $startFrame fStretch_startFrame; }\
		if ($fStretch_bakeColorPerVertex_presets[5] != \"\") { intField -e -value $endFrame fStretch_endFrame; }\
		if ($fStretch_bakeColorPerVertex_presets[6] != \"\") { textField -e -text $fStretch_bakeColorPerVertex_presets[6] fStretch_name; }\
		if ($fStretch_bakeColorPerVertex_presets[7] != \"\") { textField -e -text $fStretch_bakeColorPerVertex_presets[7] fStretch_path; }\
		if ($fStretch_bakeColorPerVertex_presets[8] != \"\") { checkBox -e -v $forceRefresh fStretch_forceRefresh; }\
\
if (($mayaYear < 2011) && (`about -li`)) { window -edit -widthHeight 430 $height fStretch_bakeColorPerVertex; } else {\
		window -edit -widthHeight 380 $height fStretch_bakeColorPerVertex; }\
		print (\"fStretch: Bake color per vertex window.\");\
	}\
\
	/* window command proc */\
\
	global proc fStretch_bakeColorPerVertex_window(){\
		$exportValue = `optionMenu -q -value fStretch_exportValue`;\
		$imageFormat = `optionMenu -q -value fStretch_imageFormat`;\
		$mapSizeX = `intField -q -value fStretch_mapSizeX`;\
		$mapSizeY = `intField -q -value fStretch_mapSizeY`;\
		$startFrame = `intField -q -value fStretch_startFrame`;\
		$endFrame = `intField -q -value fStretch_endFrame`;\
		$name = `textField -q -text fStretch_name`;\
		$path = `textField -q -text fStretch_path`;\
		$forceRefresh = `checkBox -q -v fStretch_forceRefresh`;\
\
		/* savePreset */\
\
		global string $fStretch_bakeColorPerVertex_presets[];\
		$fStretch_bakeColorPerVertex_presets[0] = $exportValue;\
		$fStretch_bakeColorPerVertex_presets[1] = $imageFormat;\
		$fStretch_bakeColorPerVertex_presets[2] = $mapSizeX;\
		$fStretch_bakeColorPerVertex_presets[3] = $mapSizeY;\
		$fStretch_bakeColorPerVertex_presets[4] = $startFrame;\
		$fStretch_bakeColorPerVertex_presets[5] = $endFrame;\
		$fStretch_bakeColorPerVertex_presets[6] = $name;\
		$fStretch_bakeColorPerVertex_presets[7] = $path;\
		$fStretch_bakeColorPerVertex_presets[8] = $forceRefresh;\
\
		/* launch the baking procedure */\
\
		string $sel[] = `ls -sl`;\
		if (size($sel) == 0) { error(\"Select a mesh.\"); }\
\
		for ($each in $sel){\
			fStretch_bakeColorPerVertex($each,$exportValue,$imageFormat,$mapSizeX,$mapSizeY,$startFrame,$endFrame,$name,$path,$forceRefresh);\
		}\
	}";
	MGlobal::executeCommand(command, false, false);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// fStretch prefs /////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	command ="global proc float fStretch_prefs(string $action, int $settingId, int $settingValue){\
		/* File's path */\
		$fileName = ( `internalVar -userPrefDir` + \"fStretchPrefs.cfg\" );\
\
		string $contentList[]; string $fileContent;\
		int $fileIdR; int $fileIdW;\
\
		/* Check if file exist */\
		if (!`filetest -e $fileName`) {\
			$fileIdW = `fopen $fileName \"w\"`;\
			if ($fileIdW == 0) { warning(\"\\nfStretch: cannot open file \" + $fileName + \" to read or set fStretch's configurations.\"); return 0; }\
/* Write default settings */\
			fwrite $fileIdW \"0;0;0;0;\";\
			fclose $fileIdW;\
			$fileIdR = `fopen $fileName \"r\"`;\
			if ($fileIdR == 0) { warning(\"\\nfStretch: cannot open file \" + $fileName + \" to read or set fStretch's configurations.\"); return 0; }\
		}\
\
		/* Read file */\
		$fileIdR = `fopen $fileName \"r\"`;\
		$fileContent = `fread $fileIdR $fileContent`;\
		fclose $fileIdR;\
		tokenize $fileContent \";\" $contentList;\
		/* read the desired setting */\
		if ($action == \"r\"){\
			return ($contentList[$settingId]);\
		}\
		/* write the desired setting */\
		if ($action == \"w\"){\
			$contentList[$settingId] = $settingValue;\
			/* write back result */\
			$fileIdW = `fopen $fileName \"w\"`;\
			if ($fileIdW == 0) { warning(\"\\nfStretch: cannot open file \" + $fileName + \" to read or set fStretch's configurations.\"); return 0; }\
			string $data = \"\";\
			for ($each in $contentList){ $data = ($data + $each + \";\"); }\
			fwrite $fileIdW $data;\
			fclose $fileIdW;\
			return 1;\
		}\
		return 0;\
	}";


	MGlobal::executeCommand(command, false, false);



	// Launch the ui

	command = "fStretch_ui;";

	MGlobal::executeCommand(command, false, false);

	return result;

}


