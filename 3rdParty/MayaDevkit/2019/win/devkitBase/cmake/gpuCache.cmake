if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4800 /wd4512 /wd4245")
	add_definitions(-D_VARIADIC_MAX=10)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-redeclared-class-member")
endif()

# framework for macosx
if (APPLE)
    set(APPLICATIONSERVICES_LIB "-framework ApplicationServices")
    set(IOKIT_LIB "-framework IOKit")
    set(PACKAGE_LIBS ${PACKAGE_LIBS} ${APPLICATIONSERVICES_LIB} ${IOKIT_LIB})
endif()