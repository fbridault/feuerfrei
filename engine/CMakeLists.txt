project(Engine)

add_subdirectory(Samples)

#permettre les calculs SSE
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -mfpmath=sse -Wall")

# Look for OpenGL
find_package(${PACKAGE} OpenGL REQUIRED)
if(OPENGL_FOUND)
  find_file (HAVE_GLEXT GL/glext.h)
  if(HAVE_GLEXT)
    add_definitions(-DGL_GLEXT_PROTOTYPES)
    message(STATUS "Found OpenGL")
  endif(HAVE_GLEXT)
  
  include_directories(${OPENGL_INCLUDE_DIR})
endif(OPENGL_FOUND)

find_package(${PACKAGE} DevIL REQUIRED)
if(IL_FOUND)
  include_directories(${IL_INCLUDE_DIR})
  add_definitions(-DILUT_USE_OPENGL)
endif(IL_FOUND)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  message(STATUS "Building a debug binary")
  add_definitions(-Wall)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

if(CMAKE_BUILD_TYPE STREQUAL Profiling)
  message(STATUS "Building a profiling binary")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -Wall")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -pg")
endif(CMAKE_BUILD_TYPE STREQUAL Profiling)

if(CMAKE_BUILD_TYPE STREQUAL Release)
  message(STATUS "Building an optimized binary")
endif(CMAKE_BUILD_TYPE STREQUAL Release)

# Source all directories
aux_source_directory(Maths MATHS_SRCS)
aux_source_directory(Physics PHYSICS_SRCS)
aux_source_directory(Scene SCENE_SRCS)
aux_source_directory(Shading SHADING_SRCS)
aux_source_directory(Utility UTILITY_SRCS)

# Manually add headers-only source files
set(UTILITY_SRCS
${UTILITY_SRCS}
Utility/CRefTable.hpp
Utility/ISingleton.hpp
Utility/Macros.hpp
Utility/Numerable.hpp
Utility/SmartPtr.hpp
)

set(ENGINE_SRCS ${MATHS_SRCS} ${PHYSICS_SRCS} ${SCENE_SRCS} ${SHADING_SRCS} ${UTILITY_SRCS} Init.cpp)

set(ENGINELIB "engine")
add_library(${ENGINELIB} SHARED ${ENGINE_SRCS}) 
target_link_libraries(${ENGINELIB} PUBLIC ${OPENGL_LIBRARIES} ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ILUT_LIBRARIES})

