if (LINUX)
  if (NOT DEFINED ENV{MAYA_LOCATION})
    message(STATUS "Please set MAYA_LOCATION environment variable to Maya installation.")
  endif()

  set(MAYA_DEFAULT_LOCATION "/usr/autodesk/maya2019")
  set(MAYA_LIB_SUFFIX "lib")

  # devkitBase library directory
 find_path(MAYA_LIBRARY_DIR libOpenMaya.so
    PATHS
      $ENV{MAYA_LOCATION}
      ${MAYA_DEFAULT_LOCATION}
    PATH_SUFFIXES
      "${MAYA_LIB_SUFFIX}/"
  DOC "Maya library path"
  )

  set(LIBS_FOR_LINUX_APP AnimSlice DeformSlice Modifiers DynSlice KinSlice ModelSlice NurbsSlice PolySlice ProjectSlice Image Shared Translators DataModel RenderModel NurbsEngine DependEngine CommandEngine IMFbase )
  set(DEVKIT_LIBRARY_DIR ${MAYA_LIBRARY_DIR})
  set(CMAKE_EXT_LINKER_FLAGS "-m64 -pthread")
endif()

