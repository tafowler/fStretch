#ifndef _clew_cl_d3d11_ext_h
#define _clew_cl_d3d11_ext_h

/*

	Include everything we need to handle the OpenCL extensions we are using.  CLEW
	doesn't have extension support, so we need to add everything we case about here.

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

#ifdef _WIN32

#include <d3d11.h>

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
					Missing definitions from cl_d3d11_ext.h
******************************************************************************/
#ifdef __OPENCL_CL_D3D11_EXT_H
#error cl_d3d11_ext.h included before OpenCL_extensions.h
#endif
#define __OPENCL_CL_D3D11_EXT_H

/******************************************************************************
 * cl_nv_d3d11_sharing                                                        */

typedef cl_uint cl_d3d11_device_source_nv;
typedef cl_uint cl_d3d11_device_set_nv;

/******************************************************************************/

// Error Codes
#define CL_INVALID_D3D11_DEVICE_NV             -1006
#define CL_INVALID_D3D11_RESOURCE_NV           -1007
#define CL_D3D11_RESOURCE_ALREADY_ACQUIRED_NV  -1008
#define CL_D3D11_RESOURCE_NOT_ACQUIRED_NV      -1009

// cl_d3d11_device_source_nv
#define CL_D3D11_DEVICE_NV                     0x4019
#define CL_D3D11_DXGI_ADAPTER_NV               0x401A

// cl_d3d11_device_set_nv
#define CL_PREFERRED_DEVICES_FOR_D3D11_NV      0x401B
#define CL_ALL_DEVICES_FOR_D3D11_NV            0x401C

// cl_context_info
#define CL_CONTEXT_D3D11_DEVICE_NV             0x401D

// cl_mem_info
#define CL_MEM_D3D11_RESOURCE_NV               0x401E

// cl_image_info
#define CL_IMAGE_D3D11_SUBRESOURCE_NV          0x401F

// cl_command_type
#define CL_COMMAND_ACQUIRE_D3D11_OBJECTS_NV    0x4020
#define CL_COMMAND_RELEASE_D3D11_OBJECTS_NV    0x4021

/******************************************************************************/

typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetDeviceIDsFromD3D11NV_fn)(
    cl_platform_id            platform,
    cl_d3d11_device_source_nv d3d_device_source,
    void *                    d3d_object,
    cl_d3d11_device_set_nv    d3d_device_set,
    cl_uint                   num_entries, 
    cl_device_id *            devices, 
    cl_uint *                 num_devices) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11BufferNV_fn)(
    cl_context     context,
    cl_mem_flags   flags,
    ID3D11Buffer * resource,
    cl_int *       errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11Texture2DNV_fn)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture2D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_mem (CL_API_CALL *clCreateFromD3D11Texture3DNV_fn)(
    cl_context        context,
    cl_mem_flags      flags,
    ID3D11Texture3D * resource,
    UINT              subresource,
    cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueAcquireD3D11ObjectsNV_fn)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    const cl_mem *   mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_0;

typedef CL_API_ENTRY cl_int (CL_API_CALL *clEnqueueReleaseD3D11ObjectsNV_fn)(
    cl_command_queue command_queue,
    cl_uint          num_objects,
    cl_mem *         mem_objects,
    cl_uint          num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event *       event) CL_API_SUFFIX__VERSION_1_0;

/*****************************************************************************
							End of cl_d3d11.h
******************************************************************************/

/*
	Up to here the file is just cl_d3d11_ext.h.  Now I'll declare a variable and
	define a macro for each function pointer type.
*/

CLEW_FUN_EXPORT		clGetDeviceIDsFromD3D11NV_fn		__clewGetDeviceIDsFromD3D11NV;
CLEW_FUN_EXPORT		clCreateFromD3D11BufferNV_fn		__clewCreateFromD3D11BufferNV;
CLEW_FUN_EXPORT		clCreateFromD3D11Texture2DNV_fn	__clewCreateFromD3D11Texture2DNV;
CLEW_FUN_EXPORT		clCreateFromD3D11Texture3DNV_fn	__clewCreateFromD3D11Texture3DNV;
CLEW_FUN_EXPORT		clEnqueueAcquireD3D11ObjectsNV_fn	__clewEnqueueAcquireD3D11ObjectsNV;
CLEW_FUN_EXPORT		clEnqueueReleaseD3D11ObjectsNV_fn	__clewEnqueueReleaseD3D11ObjectsNV;

// Don't put semi colons after your defines!
#define clGetDeviceIDsFromD3D11NV		CLEW_GET_FUN(__clewGetDeviceIDsFromD3D11NV)
#define clCreateFromD3D11BufferNV		CLEW_GET_FUN(__clewCreateFromD3D11BufferNV)
#define clCreateFromD3D11Texture2DNV	CLEW_GET_FUN(__clewCreateFromD3D11Texture2DNV)
#define clCreateFromD3D11Texture3DNV	CLEW_GET_FUN(__clewCreateFromD3D11Texture3DNV)
#define clEnqueueAcquireD3D11ObjectsNV	CLEW_GET_FUN(__clewEnqueueAcquireD3D11ObjectsNV)
#define clEnqueueReleaseD3D11ObjectsNV	CLEW_GET_FUN(__clewEnqueueReleaseD3D11ObjectsNV)

// assumes that clew is already initialized
CLEW_FUN_EXPORT int clew_cl_d3d11_extInit();

#ifdef __cplusplus
}
#endif

#endif //_WIN32

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

#endif // _clew_cl_d3d11_ext_h
