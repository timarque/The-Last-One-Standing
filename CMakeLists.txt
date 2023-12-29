#project setup
project(VR-project)

#set the minimal version of cmake and the minimal version of the c++ language
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)     
set(CMAKE_VERBOSE_MAKEFILE ON)

#define some variable
set(COMPILE_PROJECT ON CACHE BOOL "Compile project")


find_package(OpenGL REQUIRED)

#for glad library
add_library( glad STATIC 3rdParty/glad/src/glad.c)
set(GLAD_INCLUDE "3rdParty/glad/include")
target_include_directories(glad PUBLIC ${GLAD_INCLUDE})

#for assimp library
#find_package(assimp REQUIRED)
#target_include_directories(assimp PUBLIC ${ASSIMP_INCLUDE_DIRS})
include_directories("3rdParty/assimp/include")
link_directories("3rdParty/assimp/lib")
# Add Assimp library as a dependency
#add_library(assimp STATIC IMPORTED)
#set_target_properties(assimp PROPERTIES IMPORTED_LOCATION "3rdParty/assimp/lib/Debug/assimp-vc143-mtd.lib")

#for GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

#Search for glfw in case it is installed
find_package(glfw3 QUIET)
if (NOT glfw3_FOUND)
    #if glfw is not found, we will use the sources from the submodules
    add_subdirectory(3rdParty/glfw)
    include_directories(3rdParty/glfw/include/)
endif()



include_directories(3rdParty/glad/include/
                    3rdParty/glm/
                    3rdParty/stb/
                    3rdParty/assimp/include/)

                 
if(COMPILE_PROJECT)
    add_subdirectory(Project)
endif()

