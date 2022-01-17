# Set minimum cmake
cmake_minimum_required(VERSION 3.10)

# Setup project
project(openGLPlotLive-proj)

# Set OpenGL Policy
set(OpenGL_GL_PREFERENCE "GLVND")

# Add c++20 flag to compiler
set(CMAKE_CXX_STANDARD 23)

if(UNIX)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
endif()

# Add Cmake Module Path
# set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# Windows specific setup
if(WIN32)
    include_directories("${CMAKE_SOURCE_DIR}/include")
endif(WIN32)

# Find Packages
# GLM
find_package(GLM CONFIG REQUIRED)

# FREETYPE
find_package(Freetype CONFIG REQUIRED)

# GLFW3
find_package(GLFW3 CONFIG REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# X11
find_package(X11 REQUIRED)


# Set libraries
if(WIN32)
    set(LIBS freetype glfw glm::glm opengl32 )
elseif(UNIX)
    set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor dl pthread freetype GL)
endif(WIN32)

# Stop conflicting order of GLAD and GLFW
add_definitions(-DGLFW_INCLUDE_NONE)

# Add subdirectories
add_subdirectory(openGLPlotLive)
add_subdirectory(apps)
#add_subdirectory(docs)
#add_subdirectory(tests)
#add_subdirectory(third_party/google-test)
