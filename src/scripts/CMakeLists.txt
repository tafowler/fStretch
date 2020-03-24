list(APPEND scripts_src
    AEfStretchTemplate.mel
    fStretch_browse_dir.mel
    fStretch_colorPerVertexWindow.mel
    fStretch_connectMesh.mel
    fStretch_displacement.mel
    fStretch_paint.mel
    fStretch_prefs.mel
    fStretch_ui.mel
    fStretch_window.mel
    fStretch.mel
)

install(FILES ${scripts_src}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/scripts
)
