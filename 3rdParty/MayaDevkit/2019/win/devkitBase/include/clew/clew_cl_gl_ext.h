#ifndef _clew_cl_gl_ext_h
#define _clew_cl_gl_ext_h

/*
	Include everything we need to handle the OpenCL extensions we are using.  CLEW
	doesn't have extension support, so we need to add everything we care about.

	The basic steps for each method are:

	Header file:
	 - declare the function pointer type.  This should exactly match the OpenCL function.
	 - declare a variable of the defined type to hold the entry point into OpenCL
	 - define a macro which exactly matches the OpenCL function name which instead uses the function pointer

	cpp file:
	 - initialize the function pointer variable to NULL
	 - in an init method, initialize the function pointer variable to the proper value

	I will follow the same naming conventions that clew uses:
	 - function pointers called PFNCLFUNCTIONNAME

	Everything defined here should exactly match the signature of OpenCL functions,
	please don't try to do anything beyond that.  If you want to do something more,
	do it in another file.

*/
#include <clew/clew_cl.h>
#include <clew/clew_cl_gl.h>

/**********************************************************************************
 * Copyright (c) 2008-2012 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 **********************************************************************************/

/* $Revision: 11708 $ on $Date: 2010-06-13 23:36:24 -0700 (Sun, 13 Jun 2010) $ */
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
					Missing definitions from cl_gl_ext.h
******************************************************************************/
#ifdef __OPENCL_CL_GL_EXT_H
#error cl_gl_ext.h included before clew_cl_gl_ext.h
#endif
// prevent cl_gl_ext.h from being included
#define __OPENCL_CL_GL_EXT_H

// cl_gl_ext.h only applies to OSX
#ifdef __APPLE__

// Pass this as a properly to clCreateContext.
#define CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE		0x10000000

// values to pass into clGetGLContextInfoAPPLE
#define CL_CGL_DEVICE_FOR_CURRENT_VIRTUAL_SCREEN_APPLE		0x10000002
#define CL_CGL_DEVICES_FOR_SUPPORTED_VIRTUAL_SCREENS_APPLE	0x10000003

// error codes
#define CL_INVALID_GL_CONTEXT_APPLE							-1000

typedef CL_API_ENTRY cl_int (CL_API_CALL *
PFNCLGETGLCONTEXTINFOAPPLE)(cl_context     /* context */,
							void * /* platform_gl_ctx */,
							cl_gl_platform_info /* param_name */,
							size_t   /* param_value_size */,
							void *   /* param_value */,
							size_t * /* param_value_size_ret */) CL_EXT_SUFFIX__VERSION_1_0;


// TODO: I'm leaving out some stuff to support cl_khr_gl_event and clCreateImageFromIOSurface2dAPPLE

/*****************************************************************************
							End of cl_g_extl.h
******************************************************************************/

/*
	Up to here the file is just cl_gl_ext.h with the extern functions replaced by
	function pointers.  Now I'll declare a variable and define a macro for
	each function pointer type.
*/

CLEW_FUN_EXPORT		PFNCLGETGLCONTEXTINFOAPPLE		__clewGetGLContextInfoAPPLE;

// Don't put semi colons after your defines!
#define clGetGLContextInfoAPPLE		CLEW_GET_FUN(__clewGetGLContextInfoAPPLE)

// this assumes the clew is already initialized
CLEW_FUN_EXPORT int clew_cl_gl_extInit();

#endif //__APPLE__

#ifdef __cplusplus
}
#endif
//-
// ==================================================================
// Copyright 2014 Autodesk, Inc.  All rights reserved.
// 
// This computer source code  and related  instructions and comments are
// the unpublished confidential and proprietary information of Autodesk,
// Inc. and are  protected  under applicable  copyright and trade secret
// law. They may not  be disclosed to, copied or used by any third party
// without the prior written consent of Autodesk, Inc.
// ==================================================================
//+

#endif // _clew_cl_gl_ext_h
