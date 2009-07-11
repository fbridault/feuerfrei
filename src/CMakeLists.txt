#permettre les calculs SSE
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -mfpmath=sse -Wall")

include_directories(../)

# Look for OpenGL
find_package(${Package} OpenGL)
if(OPENGL_FOUND)
  find_file (HAVE_GLEXT GL/glext.h)
  if(HAVE_GLEXT)
    add_definitions(-DGL_GLEXT_PROTOTYPES)
    message(STATUS "Found OpenGL")
  endif(HAVE_GLEXT)
  
  include(${OPENGL_INCLUDE_DIR})
endif(OPENGL_FOUND)

# Look for wxWidgets
set(wxWidgets_USE_LIBS base core gl net)
find_package(wxWidgets)
if(wxWidgets_FOUND)
  message(STATUS "Found a valid installation of wxWindows")
  include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  message(STATUS "Building a debug binary")
  # Hack pour virer le strip si il existe
  list(REMOVE_ITEM wxWidgets_LIBRARIES -s)
  add_definitions(-Wall)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

if(CMAKE_BUILD_TYPE STREQUAL Profiling)
  message(STATUS "Building a profiling binary")
  # Hack pour virer le strip si il existe
  list(REMOVE_ITEM wxWidgets_LIBRARIES -s)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Wall")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -pg")
endif(CMAKE_BUILD_TYPE STREQUAL Profiling)

if(CMAKE_BUILD_TYPE STREQUAL Release)
  message(STATUS "Building an optimized binary")
endif(CMAKE_BUILD_TYPE STREQUAL Release)

# Source all directories
aux_source_directory(flames FLAMES_SRCS)
set(SOLVERS_SRCS solvers/field.cpp solvers/field3D.cpp solvers/realField3D.cpp solvers/fakeField3D.cpp solvers/LODField3D.cpp solvers/solver.cpp solvers/solver3D.cpp solvers/GSSolver3D.cpp solvers/GCSSORSolver3D.cpp solvers/GCSSORSolver3D-SSE.cpp solvers/HybridSolver3D.cpp solvers/globalField.cpp solvers/fieldThread.cpp)
set(INTERFACE_SRCS interface/GLFlameCanvas.cpp interface/luminaryDialog.cpp interface/flamesFrame.cpp interface/mainPanels.cpp interface/numTextCtrl.cpp interface/shadowsDialog.cpp)

set(RTFLAMES_SRCS flames.cpp ${FLAMES_SRCS} ${INTERFACE_SRCS} ${SOLVERS_SRCS})

set(FLAMESEXE "rtflames")
add_executable(${FLAMESEXE} ${RTFLAMES_SRCS}) 
target_link_libraries(${FLAMESEXE} ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES} engine)
set_target_properties(${FLAMESEXE} PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -DRTFLAMES_BUILD")

set(INTERFACE_SRCS interface/GLFluidsCanvas.cpp interface/fluidsFrame.cpp interface/mainPanels.cpp interface/numTextCtrl.cpp interface/solverDialog.cpp)
set(SOLVERS_SRCS solvers/field.cpp solvers/field3D.cpp solvers/realField3D.cpp solvers/fakeField3D.cpp solvers/LODField3D.cpp solvers/solver.cpp solvers/solver2D.cpp solvers/solver3D.cpp solvers/GSSolver2D.cpp solvers/GSSolver3D.cpp solvers/GCSSORSolver3D.cpp solvers/GCSSORSolver3D-SSE.cpp solvers/HybridSolver3D.cpp  solvers/benchsolver3D.cpp solvers/logResAvgSolver3D.cpp solvers/logResAvgTimeSolver3D.cpp solvers/logResSolver3D.cpp solvers/compResAvgSolver3D.cpp solvers/CGSSORSolver2D.cpp)
set(RTFLUIDS_SRCS fluids.cpp ${INTERFACE_SRCS} ${SCENE_SRCS} ${SOLVERS_SRCS})

set(FLUIDSEXE "rtfluids")
#add_executable(${FLUIDSEXE} ${RTFLUIDS_SRCS}) 
#target_link_libraries(${FLUIDSEXE} ${wxWidgets_LIBRARIES} ${OPENGL_LIBRARIES} engine)
#set_target_properties(${FLUIDSEXE} PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -DRTFLUIDS_BUILD")
