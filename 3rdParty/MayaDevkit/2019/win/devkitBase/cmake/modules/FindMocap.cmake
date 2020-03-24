set(MAYA_VERSION 2019)

if (WIN32)
	set(MAYA_DEFAULT_LOCATION "C:/Program Files/Autodesk/Maya${MAYA_VERSION}")
elseif(APPLE)
	set(MAYA_DEFAULT_LOCATION "/Applications/Autodesk/maya${MAYA_VERSION}/Maya.app/Contents/MacOS")
elseif(LINUX)
	set(MAYA_DEFAULT_LOCATION "/usr/autodesk/maya${MAYA_VERSION}")
endif()
  
set(MAYA_LIB_SUFFIX "lib")

if (WIN32)
  set(LIB_MOCAP libmocap.lib)
elseif (APPLE)
  set(LIB_MOCAP libmocap.dylib)
else ()
  set(LIB_MOCAP libmocap.a)
endif()


# ibrary directory
 find_path(MAYA_LIBRARY_DIR ${LIB_MOCAP}
    PATHS
      ${MAYA_DEFAULT_LOCATION}
      $ENV{MAYA_LOCATION}
	  $ENV{DEVKIT_LOCATION}
    PATH_SUFFIXES
      "${MAYA_LIB_SUFFIX}/"
  DOC "Maya library path"
)

find_library(LIBMOCAP_LIB NAMES libMocap PATHS ${MAYA_LIBRARY_DIR})

set(MAYA_MOCAP_LIBRARY ${MAYA_LIBRARY_DIR}/${LIB_MOCAP})


