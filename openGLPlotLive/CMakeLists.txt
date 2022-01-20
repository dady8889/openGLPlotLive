# cmake version
# cmake_minimum_required(VERSION 3.10)

# Project Name
# project(openGLPlotLive)

file(GLOB_RECURSE HEADERS_LIST CONFIGURE_DEPENDS "*.h")
file(GLOB_RECURSE SOURCES_LIST CONFIGURE_DEPENDS "*.cpp")

# Create library
add_library(openGLPlotLive ${SOURCES_LIST} ${HEADERS_LIST})

# Glad
target_sources(openGLPlotLive PUBLIC ${PROJECT_SOURCE_DIR}/include/glad.c)
target_link_libraries(openGLPlotLive PRIVATE ${LIBS})

target_include_directories(openGLPlotLive PUBLIC ${PROJECT_SOURCE_DIR}/include/)
target_include_directories(openGLPlotLive PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(openGLPlotLive INTERFACE ${PROJECT_SOURCE_DIR}/include/glad/)

# Add subdirectories
#add_subdirectory(axes)
#add_subdirectory(dataTypes)
#add_subdirectory(lines)
#add_subdirectory(shadedLines)
#add_subdirectory(plot)
#add_subdirectory(rendering)
#add_subdirectory(window)
#add_subdirectory(texts)
#add_subdirectory(util)
#add_subdirectory(interaction)
#add_subdirectory(textures)
#add_subdirectory(scatterPlot)

# Copy Shader files to build directory
add_custom_command(TARGET openGLPlotLive PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../Shaders $<TARGET_FILE_DIR:openGLPlotLive>/Shaders)
add_custom_command(TARGET openGLPlotLive PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/../textures $<TARGET_FILE_DIR:openGLPlotLive>/../textures)


