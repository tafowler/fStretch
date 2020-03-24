#ifndef _clew_cl_gl_h
#define _clew_cl_gl_h

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
					Missing definitions from cl_gl.h
******************************************************************************/
#ifdef __OPENCL_CL_GL_H
#error cl_gl.h included before clewe_cl_gl.h
#endif
// prevent cl_gl.h from being included
#define __OPENCL_CL_GL_H


typedef cl_uint     cl_gl_object_type;
typedef cl_uint     cl_gl_texture_info;
typedef cl_uint     cl_gl_platform_info;
typedef struct __GLsync *cl_GLsync;

/* cl_gl_object_type */
#define CL_GL_OBJECT_BUFFER             0x2000
#define CL_GL_OBJECT_TEXTURE2D          0x2001
#define CL_GL_OBJECT_TEXTURE3D          0x2002
#define CL_GL_OBJECT_RENDERBUFFER       0x2003

/* cl_gl_texture_info */
#define CL_GL_TEXTURE_TARGET            0x2004
#define CL_GL_MIPMAP_LEVEL              0x2005

typedef CL_API_ENTRY cl_mem (CL_API_CALL *
PFNCLCREATEFROMGLBUFFER)(cl_context     /* context */,
						cl_mem_flags   /* flags */,
						cl_GLuint      /* bufobj */,
						int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *
PFNCLCREATEFROMGLTEXTURE2D)(cl_context      /* context */,
							cl_mem_flags    /* flags */,
							cl_GLenum       /* target */,
							cl_GLint        /* miplevel */,
							cl_GLuint       /* texture */,
							cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *
PFNCLCREATEFROMGLTEXTURE3D)(cl_context      /* context */,
                        cl_mem_flags    /* flags */,
                        cl_GLenum       /* target */,
                        cl_GLint        /* miplevel */,
                        cl_GLuint       /* texture */,
                        cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *
PFNCLCREATEFROMGLRENDERBUFFER)(cl_context   /* context */,
                           cl_mem_flags /* flags */,
                           cl_GLuint    /* renderbuffer */,
                           cl_int *     /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *
PFNCLGETGLOBJECTINFO)(cl_mem                /* memobj */,
                  cl_gl_object_type *   /* gl_object_type */,
                  cl_GLuint *              /* gl_object_name */) CL_API_SUFFIX__VERSION_1_0;
                  
typedef CL_API_ENTRY cl_int (CL_API_CALL *
PFNCLGETGLTEXTUREINFO)(cl_mem               /* memobj */,
                   cl_gl_texture_info   /* param_name */,
                   size_t               /* param_value_size */,
                   void *               /* param_value */,
                   size_t *             /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *
PFNCLENQUEUEACQUIREGLOBJECTS)(cl_command_queue      /* command_queue */,
                          cl_uint               /* num_objects */,
                          const cl_mem *        /* mem_objects */,
                          cl_uint               /* num_events_in_wait_list */,
                          const cl_event *      /* event_wait_list */,
                          cl_event *            /* event */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *
PFNCLENQUEUERELEASEGLOBJECTS)(cl_command_queue      /* command_queue */,
                          cl_uint               /* num_objects */,
                          const cl_mem *        /* mem_objects */,
                          cl_uint               /* num_events_in_wait_list */,
                          const cl_event *      /* event_wait_list */,
                          cl_event *            /* event */) CL_API_SUFFIX__VERSION_1_0;

/* cl_khr_gl_sharing extension  */

#define cl_khr_gl_sharing 1

typedef cl_uint     cl_gl_context_info;

/* Additional Error Codes  */
#define CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR  -1000

/* cl_gl_context_info  */
#define CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR    0x2006
#define CL_DEVICES_FOR_GL_CONTEXT_KHR           0x2007

/* Additional cl_context_properties  */
#define CL_GL_CONTEXT_KHR                       0x2008
#define CL_EGL_DISPLAY_KHR                      0x2009
#define CL_GLX_DISPLAY_KHR                      0x200A
#define CL_WGL_HDC_KHR                          0x200B
#define CL_CGL_SHAREGROUP_KHR                   0x200C

typedef CL_API_ENTRY cl_int (CL_API_CALL *
PFNCLGETGLCONTEXTINFOKHR)(const cl_context_properties * /* properties */,
                      cl_gl_context_info            /* param_name */,
                      size_t                        /* param_value_size */,
                      void *                        /* param_value */,
                      size_t *                      /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetGLContextInfoKHR_fn)(
    const cl_context_properties * properties,
    cl_gl_context_info            param_name,
    size_t                        param_value_size,
    void *                        param_value,
    size_t *                      param_value_size_ret);

/* cl_khr_gl_event */
typedef CL_API_ENTRY cl_event (CL_API_CALL *
PFNCLCREATEEVENTFROMGLSYNCKHR)(cl_context       /* context */,
                            cl_GLsync           /* gl_GLsync */,
                            cl_int *            /* errcode_ret*/) CL_API_SUFFIX__VERSION_1_1;

#define CL_COMMAND_GL_FENCE_SYNC_OBJECT_KHR     0x200D

/*****************************************************************************
							End of cl_gl.h
******************************************************************************/

/*
	Up to here the file is just cl_gl.h with the extern functions replaced by
	function pointers.  Now I'll declare a variable and define a macro for
	each function pointer type.
*/

CLEW_FUN_EXPORT		PFNCLCREATEFROMGLBUFFER			__clewCreateFromGLBuffer;
CLEW_FUN_EXPORT		PFNCLCREATEFROMGLTEXTURE2D		__clewCreateFromGLTexture2D;
CLEW_FUN_EXPORT		PFNCLCREATEFROMGLTEXTURE3D		__clewCreateFromGLTexture3D;
CLEW_FUN_EXPORT		PFNCLCREATEFROMGLRENDERBUFFER	__clewCreateFromGLRenderBuffer;
CLEW_FUN_EXPORT		PFNCLGETGLOBJECTINFO			__clewGetGLObjectInfo;
CLEW_FUN_EXPORT		PFNCLGETGLTEXTUREINFO			__clewGetGLTextureInfo;
CLEW_FUN_EXPORT		PFNCLENQUEUEACQUIREGLOBJECTS	__clewEnqueueAcquireGLObjects;
CLEW_FUN_EXPORT		PFNCLENQUEUERELEASEGLOBJECTS	__clewEnqueueReleaseGLObjects;
CLEW_FUN_EXPORT		PFNCLGETGLCONTEXTINFOKHR		__clewGetGLContextInfoKHR;
CLEW_FUN_EXPORT     PFNCLCREATEEVENTFROMGLSYNCKHR   __clewCreateEventFromGLsyncKHR;

// Don't put semi colons after your defines!
#define clCreateFromGLBuffer		CLEW_GET_FUN(__clewCreateFromGLBuffer)
#define clCreateFromGLTexture2D		CLEW_GET_FUN(__clewCreateFromGLTexture2D)
#define clCreateFromGLTexture3D		CLEW_GET_FUN(__clewCreateFromGLTexture3D)
#define clCreateFromGLRenderbuffer	CLEW_GET_FUN(__clewCreateFromGLRenderBuffer)
#define clGetGLObjectInfo			CLEW_GET_FUN(__clewGetGLObjectInfo)
#define clGetGLTextureInfo			CLEW_GET_FUN(__clewGetGLTextureInfo)
#define clEnqueueAcquireGLObjects	CLEW_GET_FUN(__clewEnqueueAcquireGLObjects)
#define clEnqueueReleaseGLObjects	CLEW_GET_FUN(__clewEnqueueReleaseGLObjects)
#define clGetGLContextInfoKHR		CLEW_GET_FUN(__clewGetGLContextInfoKHR)
#define clCreateEventFromGLsyncKHR  CLEW_GET_FUN(__clewCreateEventFromGLsyncKHR)

// this assumes the clew is already initialized
CLEW_FUN_EXPORT int clew_cl_glInit();

#ifdef __cplusplus
}
#endif
//-
// ==================================================================
// Copyright 2013 Autodesk, Inc.  All rights reserved.
// 
// This computer source code  and related  instructions and comments are
// the unpublished confidential and proprietary information of Autodesk,
// Inc. and are  protected  under applicable  copyright and trade secret
// law. They may not  be disclosed to, copied or used by any third party
// without the prior written consent of Autodesk, Inc.
// ==================================================================
//+

#endif // _clew_cl_gl_h
