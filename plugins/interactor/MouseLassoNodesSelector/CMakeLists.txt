INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipOGLInclude} ${TulipGUIInclude} ${OPENGL_INCLUDE_DIR})

SET(LIB_SRCS
    MouseLassoNodesSelector.cpp
)

QTX_ADD_RESOURCES(LIB_RCC_SRCS
    MouseLassoNodesSelectorResource.qrc
)

ADD_LIBRARY(MouseLassoNodesSelector-${TulipVersion} SHARED ${LIB_SRCS} ${LIB_RCC_SRCS})
TARGET_LINK_LIBRARIES(MouseLassoNodesSelector-${TulipVersion} ${LibTulipCoreName} ${LibTulipOGLName} ${LibTulipGUIName} ${OPENGL_gl_LIBRARY})

TULIP_INSTALL_PLUGIN(MouseLassoNodesSelector-${TulipVersion} ${TulipInteractorPluginsInstallDir})
