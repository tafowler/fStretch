set(MAYA_VERSION 2019)

if (WIN32)
	set(MAYA_DEFAULT_LOCATION "C:/Program Files/Autodesk/Maya${MAYA_VERSION}")
    set(ADSKXGEN_LIB AdskXGen.lib)
elseif(APPLE)
	set(MAYA_DEFAULT_LOCATION "/Applications/Autodesk/maya${MAYA_VERSION}")
    set(ADSKXGEN_LIB libAdskXGen.dylib)
elseif(LINUX)
	set(MAYA_DEFAULT_LOCATION "/usr/autodesk/maya${MAYA_VERSION}")
    set(ADSKXGEN_LIB libAdskXGen.so)
endif()

set(XGEN_PLUGIN plug-ins/xgen)
find_path(XGEN_LIB_DIR ${ADSKXGEN_LIB}
        HINTS
            ${MAYA_LOCATION}/${XGEN_PLUGIN}
            $ENV{MAYA_LOCATION}/${XGEN_PLUGIN}
            ${MAYA_DEFAULT_LOCATION}/${XGEN_PLUGIN}
        PATH_SUFFIXES
            "lib/"
        DOC
            "XGen library path"
)


find_path(XGEN_INCLUDE_DIR
                xgen/src/xgcore/XgConfig.h
        HINTS
            ${MAYA_LOCATION}/${XGEN_PLUGIN}
            $ENV{MAYA_LOCATION}/${XGEN_PLUGIN}
            ${MAYA_DEFAULT_LOCATION}/${XGEN_PLUGIN}
        PATH_SUFFIXES
            "include/"
        DOC
            "XGen include path"
)

if (XGEN_LIB_DIR AND XGEN_INCLUDE_DIR)
    set(XGEN_FOUND TRUE)
endif()



