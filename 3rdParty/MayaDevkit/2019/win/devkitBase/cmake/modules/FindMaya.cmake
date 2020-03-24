set(MAYA_VERSION 2019)

if (WIN32)
	set(MAYA_DEFAULT_LOCATION "C:/Program Files/Autodesk/Maya${MAYA_VERSION}")
	set(OPEN_MAYA OpenMaya.lib)
    set(LIB_SUFFIX "lib/")
elseif(APPLE)
	set(MAYA_DEFAULT_LOCATION "/Applications/Autodesk/maya${MAYA_VERSION}")
	set(OPEN_MAYA libOpenMaya.dylib)
    set(LIB_SUFFIX "Maya.app/Contents/MacOS")
elseif(LINUX)
	set(MAYA_DEFAULT_LOCATION "/usr/autodesk/maya${MAYA_VERSION}")
	set(OPEN_MAYA libOpenMaya.so)
    set(LIB_SUFFIX "lib/")
endif()

find_path(MAYA_BASE_DIR
        include/maya/MFn.h
		HINTS
			${MAYA_LOCATION}
			$ENV{MAYA_LOCATION}
			${MAYA_DEFAULT_LOCATION}
		
        DOC
            "Maya's base path"
		)

find_path(MAYA_INCLUDE_DIR
        maya/MFn.h
		HINTS
			${MAYA_LOCATION}
			$ENV{MAYA_LOCATION}
			${MAYA_DEFAULT_LOCATION}
		PATH_SUFFIXES
            "include/"
        DOC
            "Maya's include path"
		)
		
find_path(MAYA_LIBRARY_DIR
            ${OPEN_MAYA}
        HINTS
            ${MAYA_LOCATION}
            $ENV{MAYA_LOCATION}
            ${MAYA_DEFAULT_LOCATION}
        PATH_SUFFIXES
            ${LIB_SUFFIX}
        DOC
            "Maya's libraries path"
    )
	
if (MAYA_INCLUDE_DIR AND MAYA_LIBRARY_DIR)
	set(MAYA_FOUND TRUE)
endif()
	
