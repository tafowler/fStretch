set(TARGET_NAME fStretch)

add_library(${TARGET_NAME} MODULE)

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME} 
    PRIVATE
        plugin.cpp
        fStretch.cpp
        fStretchMaths.cpp
        fStretchWrap.cpp
)

# -----------------------------------------------------------------------------
# compile definitions
# -----------------------------------------------------------------------------
target_compile_definitions(${TARGET_NAME}
    PRIVATE
        FSTRETCH_PLUGIN_EXPORT
)

# -----------------------------------------------------------------------------
# include directories
# -----------------------------------------------------------------------------
target_include_directories(${TARGET_NAME}
    PUBLIC
        ${MAYA_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_BINARY_DIR}/include
)

# -----------------------------------------------------------------------------
# link libraries
# -----------------------------------------------------------------------------
target_link_libraries(${TARGET_NAME}
    PRIVATE
        ${MAYA_LIBRARIES}
)

# -----------------------------------------------------------------------------
# properties
# -----------------------------------------------------------------------------
maya_set_plugin_properties(${TARGET_NAME})

add_subdirectory(scripts)

# -----------------------------------------------------------------------------
# install
# -----------------------------------------------------------------------------
install(TARGETS
    ${TARGET_NAME}
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_PREFIX}/plugin
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_PREFIX}/plugin
)

if(IS_WINDOWS)
    install(FILES $<TARGET_PDB_FILE:${TARGET_NAME}> DESTINATION ${CMAKE_INSTALL_PREFIX}/plugin OPTIONAL)
endif()
