# -Find D3DX11Effects location
#
# Variables that will be defined:
# D3DX11Effects_LOCATION      Path to install D3DX11Effects library
# D3DX11Effects_INCLUDE_DIR   Path to the D3DX11Effects include directory
# D3DX11Effects_LIBRARY_DIR   Path to the D3DX11Effects lib directory
#

## User can set D3DX11Effects location here directory
# set D3DX11Effects_LOCATION 
# set(D3DX11Effects_LOCATION "")

if(NOT DEFINED ENV{D3DX11Effects_LOCATION} AND NOT DEFINED D3DX11Effects_LOCATION)
	message("Need to install D3DX11Effects and set environment variable D3DX11Effects_LOCATION as the installation path!")
else()
	find_library(D3DX11EFFECTS_LIB_DIR NAMES D3DX11Effects
			PATHS
				$ENV{D3DX11Effects_LOCATION}/lib
				${D3DX11Effects_LOCATION}/lib
			DOC
				"D3DX11Effects's lib path"
			)
	
		
	find_path(D3DX11Effects_INCLUDE_DIR DX11/d3dx11effect.h
		HINTS 
			$ENV{D3DX11Effects_LOCATION}
			${D3DX11Effects_LOCATION}
		PATH_SUFFIXES
            "include/"
		DOC
            "D3DX11Effects's include path"
		 )
		 
	if (D3DX11EFFECTS_LIB_DIR AND D3DX11Effects_INCLUDE_DIR)
		set(D3DX11EFFECTS_FOUND TRUE)
	endif()
endif()
  
