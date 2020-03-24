# -Find Arnold module
#
# Variables that will be defined:
# MTOA_PATH          Path to install Arnold SDK
# MTOA_INCLUDE_DIR   Path to the Arnold's include directory
# MTOA_BIN_DIR       Path to the Arnold's bin directory
#

# set MTOA_PATH 
# set(MTOA_PATH "")

if(NOT DEFINED ENV{MTOA_LOCATION} AND NOT DEFINED MTOA_LOCATION)
  message("Need to install Solid Angle MtoA SDK and set environment variable MTOA_LOCATION as the installation path!")
else()
  find_path(MTOA_INCLUDE_DIR NAMES utils/MtoaLog.h
  PATHS
    ${MTOA_LOCATION}
    $ENV{MTOA_LOCATION}
  PATH_SUFFIXES
    "include/"
  DOC "MtoA include path"
)

	if (MTOA_INCLUDE_DIR)
		set(MTOA_FOUND TRUE)
	endif()
  
  if (WIN32)
    set(LIB_SUFFIX "lib/")
  elseif (APPLE)
    set(LIB_SUFFIX "bin/")
  else()
    set(LIB_SUFFIX "lib/")
  endif()
  
find_library(MTOA_LIB_DIR NAMES mtoa_api
  PATHS
    ${MTOA_LOCATION}/${LIB_SUFFIX}
    $ENV{MTOA_LOCATION}/${LIB_SUFFIX}
  DOC "mtoa lib path"
)

endif()

