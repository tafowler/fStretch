# -Find Arnold module
#
# Variables that will be defined:
# ARNOLD_LOCATION      Path to install Arnold SDK
# ARNOLD_INCLUDE_DIR   Path to the Arnold's include directory
# ARNOLD_BIN_DIR       Path to the Arnold's bin directory
#

# set ARNOLD_LOCATION 
# set(ARNOLD_LOCATION "")

if(NOT DEFINED ENV{ARNOLD_LOCATION} AND NOT DEFINED ARNOLD_LOCATION)
  message("Need to install Arnold SDK 4.2.4 and set environment variable ARNOLD_LOCATION as the installation path!")
else()
  find_path(ARNOLD_INCLUDE_DIR NAMES ai.h
  PATHS
    ${ARNOLD_LOCATION}
    $ENV{ARNOLD_LOCATION}
  PATH_SUFFIXES
    "include/"
  DOC "Arnold include path"
)

	if (ARNOLD_INCLUDE_DIR)
		set(ARNOLD_FOUND TRUE)
	endif()
  
  if (WIN32)
    set(ARNOLD_BIN_SUFFIX "lib")
	set(AILIB "ai.lib")
  elseif (APPLE)
    set(ARNOLD_BIN_SUFFIX "bin")
	set(AILIB "libai.dylib")
  else()
    set(ARNOLD_BIN_SUFFIX "bin")
	set(AILIB "libai.so")
  endif()
  
  find_path(ARNOLD_BIN_DIR ${AILIB}
  PATHS
    ${ARNOLD_LOCATION}
    $ENV{ARNOLD_LOCATION}
  PATH_SUFFIXES
    "${ARNOLD_BIN_SUFFIX}/"
  DOC "Arnold lib path"
)
  find_library(AI_LIB NAMES ${AILIB} PATHS ${ARNOLD_BIN_DIR})
  set(ARNOLD_LIBRARY ${AI_LIB})
endif()

