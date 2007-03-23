#Change PROJECT_NAME to the name of your project
PROJECT(REAL-TIME_FLAMES)

#include some macros from another file...  
INCLUDE (${CMAKE_ROOT}/Modules/FindPNG.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/FindwxWidgets.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/FindMFC.cmake)

# Look for OpenGL
FIND_PACKAGE(${Package} OpenGL)
IF(OPENGL_FOUND)
  FIND_FILE (HAVE_GLEXT GL/glext.h)
  IF(HAVE_GLEXT)
    ADD_DEFINITIONS(-DGL_GLEXT_PROTOTYPES)
    MESSAGE(STATUS "Found OpenGL")
  ENDIF(HAVE_GLEXT)
  
  INCLUDE(${OPENGL_INCLUDE_DIR})
ENDIF(OPENGL_FOUND)

# Look for Cg
FIND_FILE(CGHTEST NAMES Cg/cg.h Cg/cgGL.h)
IF(CGHTEST)
  FIND_LIBRARY(CGLIBTEST NAMES Cg CgGL)
  IF(CGLIBTEST)
    SET(CG_LIBRAIRIES Cg CgGL)
    MESSAGE(STATUS "Found NVidia Cg devkit")
  ELSE(CGLIBTEST)
    MESSAGE(FATAL_ERROR "Missing NVidia CG libraries")
  ENDIF(CGLIBTEST)
ELSE(CGHTEST)
  MESSAGE(FATAL_ERROR "Missing NVidia CG headers")
ENDIF(CGHTEST)

# Look for wxWidgets
SET(wxWidgets_USE_LIBS base core gl net)
FIND_PACKAGE(wxWidgets)
IF(wxWidgets_FOUND)
  MESSAGE(STATUS "Found a valid installation of wxWindows")
  INCLUDE(${wxWidgets_USE_FILE})
ENDIF(wxWidgets_FOUND)

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
  MESSAGE(STATUS "Building a debug binary")
  # Hack pour virer le strip si il existe
  LIST(REMOVE_ITEM wxWidgets_LIBRARIES -s)
  ADD_DEFINITIONS(-Wall)
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)

IF(CMAKE_BUILD_TYPE STREQUAL Profiling)
  MESSAGE(STATUS "Building a profiling binary")
  # Hack pour virer le strip si il existe
  LIST(REMOVE_ITEM wxWidgets_LIBRARIES -s)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Wall")
  SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -pg")
ENDIF(CMAKE_BUILD_TYPE STREQUAL Profiling)

IF(CMAKE_BUILD_TYPE STREQUAL Release)
  MESSAGE(STATUS "Building an optimized binary")
ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)

# Source all directories
AUX_SOURCE_DIRECTORY(flames FLAMES_SRCS)
AUX_SOURCE_DIRECTORY(interface INTERFACE_SRCS)
AUX_SOURCE_DIRECTORY(scene SCENE_SRCS)
AUX_SOURCE_DIRECTORY(shaders SHADERS_SRCS)
SET(SOLVERS_SRCS solvers/field.cpp solvers/field3D.cpp solvers/realField3D.cpp solvers/fakeField3D.cpp solvers/LODField3D.cpp solvers/solver.cpp solvers/solver3D.cpp solvers/GSSolver3D.cpp solvers/GCSSORSolver3D.cpp solvers/HybridSolver3D.cpp solvers/globalField.cpp)
SET(INTERFACE_SRCS interface/GLFlameCanvas.cpp interface/flameDialog.cpp interface/flamesFrame.cpp interface/mainPanels.cpp interface/numTextCtrl.cpp interface/shadowsDialog.cpp interface/solverDialog.cpp)

SET(RTFLAMES_SRCS flames.cpp ${FLAMES_SRCS} ${INTERFACE_SRCS} ${SCENE_SRCS} ${SHADERS_SRCS} ${SOLVERS_SRCS})

SET(FLAMESEXE "rtflames")
ADD_EXECUTABLE(${FLAMESEXE} ${RTFLAMES_SRCS}) 
TARGET_LINK_LIBRARIES(${FLAMESEXE} ${wxWidgets_LIBRARIES} ${CG_LIBRAIRIES} ${OPENGL_LIBRARIES})
SET_TARGET_PROPERTIES(${FLAMESEXE} PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -DRTFLAMES_BUILD")

SET(INTERFACE_SRCS interface/GLFluidsCanvas.cpp interface/fluidsFrame.cpp interface/mainPanels.cpp interface/numTextCtrl.cpp interface/solverDialog.cpp)
SET(SCENE_SRCS scene/camera.cpp scene/graphicsFn.cpp)

SET(SOLVERS_SRCS solvers/field.cpp solvers/field3D.cpp solvers/realField3D.cpp solvers/fakeField3D.cpp solvers/LODField3D.cpp solvers/solver.cpp solvers/solver2D.cpp solvers/solver3D.cpp solvers/GSSolver2D.cpp solvers/GSSolver3D.cpp solvers/GCSSORSolver3D.cpp solvers/HybridSolver3D.cpp  solvers/benchsolver3D.cpp solvers/logResAvgSolver3D.cpp solvers/logResAvgTimeSolver3D.cpp solvers/logResSolver3D.cpp)
SET(RTFLUIDS_SRCS fluids.cpp ${INTERFACE_SRCS} ${SCENE_SRCS} ${SOLVERS_SRCS})

SET(FLUIDSEXE "rtfluids")
ADD_EXECUTABLE(${FLUIDSEXE} ${RTFLUIDS_SRCS}) 
TARGET_LINK_LIBRARIES(${FLUIDSEXE} ${wxWidgets_LIBRARIES} ${CG_LIBRAIRIES} ${OPENGL_LIBRARIES})
SET_TARGET_PROPERTIES(${FLUIDSEXE} PROPERTIES COMPILE_FLAGS -DRTFLUIDS_BUILD)
