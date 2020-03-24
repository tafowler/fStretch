
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


#include <maya/MIntArray.h>
#include <maya/MItMeshPolygon.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////// MATH PROC DECLARATION ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// All those proc are in fStretch_math.cpp

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


// ################################################################################################
// ######################### GET WRAP VALUES (for one triangle and a point) #######################
// ################################################################################################

MVector getWrapValues(MPoint A, MPoint B, MPoint C, MVector faceNormal, MVector point){
	// Find closest point on plane and it's distance
    MPoint closestPointOnPlaneResults = closestPointOnPlane(faceNormal,A,point);
    MVector closestPointOnPlane;
	closestPointOnPlane.x = closestPointOnPlaneResults.x;
	closestPointOnPlane.y = closestPointOnPlaneResults.y;
	closestPointOnPlane.z = closestPointOnPlaneResults.z;

    // Find closest point on an edge determined by the two first vertex of the face $A and $B following the vectors of AC*BD
    // Find  AC vector and normalized it. Add that to the closest point on face so we have two point and form a line in the direction of the vector AC.
    MVector ac = (C-A);	ac = normalizeVector(ac);

    MVector linePt = (closestPointOnPlane + ac);
    
    MVector edgeIntersection = intersectTwoLines(A,B,(closestPointOnPlane),linePt);
    float planeToEdgeDistance = distance3D(edgeIntersection,closestPointOnPlane);
    float edgeToVertexDistance = distance3D(edgeIntersection,A);

    // output the values we found
    float U = (dotP( ((A-B) / noZero(distance3D(A,B)) ) , (A-edgeIntersection) )) / noZero(distance3D(A,B)) ;
    float V = (dotP( (closestPointOnPlane - linePt) / noZero(distance3D(closestPointOnPlane,linePt)) , (edgeIntersection - closestPointOnPlane)) / noZero(distance3D(A,C)) );
    float N = closestPointOnPlaneResults.w;
    
	MVector result;
	result.x = U;
	result.y = V;
	result.z = N;

    return result;
}

// ################################################################################################
// ######################### GET WRAP RESULT (for one triangle get the point) #####################
// ################################################################################################

MVector getWrapResult(MPoint A, MPoint B, MPoint C, MVector faceNormal, float U, float V, float N)
{
    MVector p;
    // Go at the given U % on the AB edge
    p = (A + ((B - A) * U));

    // find ac vector and normalized it
	MVector ac = (C-A); ac = normalizeVector(ac);

	// project our point in the direction of ac vector given the length V
	p = (p + (ac * ( V * distance3D(A,C) ) ));

    // project our point in the direction of the normal given the length of N
    p = (p + (faceNormal * N));

    return p;
}

// proc to order an array of faces which form one layer/radius. (If they do not all have neighboors vertex, return the initial array)
// then with this array, decompose it into tris and return the tri in an array

MIntArray orderFacesByNeighboors_decomposeIntoTriArray(MIntArray facesArray, MIntArray vtxPerFaceList, int oneTriPerFace){

	MIntArray facesArrayModified = facesArray;


    MIntArray newFacesArray; newFacesArray.clear();
	MIntArray newVtxPerFaceList; newVtxPerFaceList.clear();
	newVtxPerFaceList.append(vtxPerFaceList[0]);

    // Put the first face in our $newFacesArray
    for (int v=0; v<vtxPerFaceList[0]; v++){
		newFacesArray.append(facesArrayModified[v]);
		facesArrayModified[v] = -1;
    }


    int i=1; int facesFound = 1; int foundANewOne = 1; int howManyNeighboorsPoints = 0;
    int currentIndexIn_newFacesArray = 0;

	int tryAgain = 1; int numberOfTries = 0;  
	while (tryAgain){
		while ( (facesFound != (vtxPerFaceList.length()) ) && (foundANewOne) ){
			foundANewOne=0; int currentIndexIn_facesArrayModified = vtxPerFaceList[0];
			for (i=1; i<( (int)vtxPerFaceList.length() ); i++ ){

				// find a faces next to our faces .. compare points

				howManyNeighboorsPoints = 0;
				int b; int c;

				for (b=0; b<(vtxPerFaceList[currentIndexIn_newFacesArray]); b++){
					for (c=0; c<vtxPerFaceList[i]; c++){

						if (newFacesArray[newFacesArray.length() -1 - b] == facesArrayModified[currentIndexIn_facesArrayModified+c]) {

							// We found a neighboor point

							howManyNeighboorsPoints++;
						}
					}
				}
				// We just finished scanning one the face and compare it's points to our last faces in $newFacesArray
				// If it's a neighboor then proceed !

				if (howManyNeighboorsPoints > 1){

					 // the $i's faces in $facesArrayModified is next to our last faces in our $newFacesArray.
					 // put that faces in our $newFacesArray
					 int e;
					 for (e=0; e<vtxPerFaceList[i]; e++){
						newFacesArray.append(facesArrayModified[currentIndexIn_facesArrayModified+e]);
						facesArrayModified[currentIndexIn_facesArrayModified+e] = -1;
					 }

					// I add one in the newFacesArray... it's the i;
					currentIndexIn_newFacesArray = i;
					newVtxPerFaceList.append(vtxPerFaceList[i]);

					 // remove that faces from the $facesArrayModified
	        
					 foundANewOne = 1;
					 facesFound++;
				}

				currentIndexIn_facesArrayModified = (currentIndexIn_facesArrayModified + vtxPerFaceList[i]);
			}
		}
	    
		// If we are on the corner of a geometry it might do the "U effect" so we have to turn around the matrix if we're still unable to find the neighboors.
		// ie:
		//
		// [3] [4] [5]
		// [2]     [6]
		// [1]     [7]
		//
		// in this situation, if the algorythm found : 2,3,4,5,6,7 it will never find the 1 after. If we turn around the matrix:7,6,5,4,3,2 it'll find the 1 and append it.    
	    
	    
		// We ordered our faces... split them an array of triangles now.
	    
    
		numberOfTries++;

		if (facesFound != (vtxPerFaceList.length())) { 
			if (numberOfTries == 1){
				// We tried one time and it did not work. Return the matrix around in case we're running into the U effect and try again.
				MIntArray newVtxPerFaceList_copy = newVtxPerFaceList;
				MIntArray newFacesArray_copy = newFacesArray;

				int i;
				i = ((int)newVtxPerFaceList.length() -1);
				for (int b=0; b<(int)newVtxPerFaceList_copy.length();b++){
					newVtxPerFaceList[i] = newVtxPerFaceList_copy[b];
					i--;
				}

				i=((int)newFacesArray.length() -1);
				for (int b=0; b<(int)newFacesArray_copy.length(); b++){
					newFacesArray[i] = newFacesArray_copy[b];
					i--;
				}

				// We put $foundANewOne = 1 otherwise our loop won't re-evaluate
				foundANewOne=1;
				// doesnt work, turn around
			}
			else {
				// They're not all neighboors.. return the original array and go on
				newVtxPerFaceList = vtxPerFaceList;
				newFacesArray = facesArray;
				tryAgain = 0;
		   }
		}
		else {
			// We succeed .. stop trying and go on.
			tryAgain = 0;
		}
    }

    // From now on, work with $newFacesArray
    // Decompose $newFacesArray into an array of triangles   

	MIntArray triArray; triArray.clear();
    currentIndexIn_newFacesArray = 0;

	for (i=0; i<(int)newVtxPerFaceList.length(); i++){

        // If it's a tri, return it directly
        if (newVtxPerFaceList[i] == 3) {

			triArray.append(newFacesArray[currentIndexIn_newFacesArray+0]);
			triArray.append(newFacesArray[currentIndexIn_newFacesArray+1]);
			triArray.append(newFacesArray[currentIndexIn_newFacesArray+2]);
        }
        else{

            // If it's a quad or 5 side+ faces, return 2 tris
			triArray.append(newFacesArray[currentIndexIn_newFacesArray+0]);
            triArray.append(newFacesArray[currentIndexIn_newFacesArray+1]);
            triArray.append(newFacesArray[currentIndexIn_newFacesArray+2]);
            
            if (!oneTriPerFace){

				triArray.append(newFacesArray[currentIndexIn_newFacesArray+0]);
                triArray.append(newFacesArray[currentIndexIn_newFacesArray+2]);
                triArray.append(newFacesArray[currentIndexIn_newFacesArray+3]);
            }
        }

        currentIndexIn_newFacesArray += newVtxPerFaceList[i];
    }
   
    return triArray;

}

///////////////////////// proc to draw tris ////////////////////////

#include <maya/MThreadUtils.h>

void drawTri(MIntArray orderedFaces, int i){

	for (int triIndex=0; triIndex<((int)orderedFaces.length() /3); triIndex++){

		MString command;
		MString a,b,c;

		command = ("string $mesh = \"pCube1\";");

		MGlobal::executeCommandStringResult(command, false, false, NULL);

		a = ("$tri[0] = ");
		b = (orderedFaces[triIndex*3]);
		c = (";");
		command = (a+b+c);
		MGlobal::executeCommandStringResult(command, false, false, NULL);

		a = ("$tri[1] = ");
		b = (orderedFaces[triIndex*3+1]);
		c = (";");
		command = (a+b+c);
		MGlobal::executeCommandStringResult(command, false, false, NULL);

		a = ("$tri[2] = ");
		b = (orderedFaces[triIndex*3+2]);
		c = (";");
		command = (a+b+c);
		MGlobal::executeCommandStringResult(command, false, false, NULL);

		MGlobal::executeCommandStringResult("float $tri_1[] = `xform -q -t ($mesh + \".vtx[\" + $tri[0] + \"]\")`;", false, false, NULL);
		MGlobal::executeCommandStringResult("float $tri_2[] = `xform -q -t ($mesh + \".vtx[\" + $tri[1] + \"]\")`;", false, false, NULL);
		MGlobal::executeCommandStringResult("float $tri_3[] = `xform -q -t ($mesh + \".vtx[\" + $tri[2] + \"]\")`;", false, false, NULL);
	    
		MGlobal::executeCommandStringResult("polyCreateFacet -ch on -tx 1 -s 1 -p $tri_1[0] $tri_1[1] $tri_1[2] -p $tri_2[0] $tri_2[1] $tri_2[2] -p $tri_3[0] $tri_3[1] $tri_3[2];", false, false, NULL);

		a = ("rename \"vertex_");
		b = i;
		c = ("_tri\";");
		command = (a+b+c);
		MGlobal::executeCommandStringResult(command, false, false, NULL);

	}
}

	// proc to select the tris to take for each level of our wrap smooth (and the first level too ! )
	MIntArray trisPerRadius(int currentRadius, int itemCount, int smoothRadius, float itemsPerRadius, float centralFactor){

    // our current radius in our while loop. It starts at it's highest and ends at it's lowest. This means that 0 is it's last pass and the farest tris in the smooth processus.
	MIntArray indexes; indexes.clear();
    
    // We do our calculation for our central factor
    float smoothRadiusFloat = smoothRadius; float currentRadiusFloat = currentRadius;
    float centralFactorMult = (currentRadiusFloat / noZero(smoothRadiusFloat));
    centralFactorMult *= centralFactor;
    
    int additionalFacesToTake = ((itemCount - (itemsPerRadius * itemCount)) * centralFactorMult);
    
    // We do our calculation to divide equally all our items in our current radius with $itemsPerRadius.
    float newItemCount = (additionalFacesToTake + (itemCount*itemsPerRadius));

	for (float i=0; i<newItemCount; i+=1){
		indexes.append(round( (itemCount/noZero(newItemCount)) * i) );
    }
    return indexes;
}
