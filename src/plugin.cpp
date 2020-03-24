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

#include <maya/MFnPlugin.h>
#include <maya/MStatus.h>

#include "fStretch.h"

MStatus initializePlugin( MObject obj )
{
    MStatus result;

    MFnPlugin plugin( obj, "Matthieu Fiorilli", "2.00", "Any");

    result = plugin.registerNode( "fStretch", fStretch::id, fStretch::creator, fStretch::initialize, MPxNode::kDeformerNode );

/*
	// Launch the ui
	command = "fStretch_ui;";
	MGlobal::executeCommand(command, false, false);
*/

	return result;
}

MStatus uninitializePlugin( MObject obj)
{
    MStatus result;
    MFnPlugin plugin( obj );

	result = plugin.deregisterNode( fStretch::id );

	return result;
}
