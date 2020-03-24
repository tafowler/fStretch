# For Windows, user needs to install Microsfot DirectX SDK and set up the library and include path

if (WIN32)
	if (NOT DEFINED ENV{MICROSOFT_DIRECTX_SDK_LOCATION})
		message("Microsoft DirectX SDK needs to be installed, and set the environment variable MICROSOFT_DIRECTX_SDK_LOCATION.")
	else()
		set(DIRECTX_LIB_DIR $ENV{MICROSOFT_DIRECTX_SDK_LOCATION}/lib/x64)
		set(DIRECTX_INCLUDE_DIR $ENV{MICROSOFT_DIRECTX_SDK_LOCATION}/include)
		set(DIRECTXSDK_FOUND TRUE)
		
	endif()
endif ()