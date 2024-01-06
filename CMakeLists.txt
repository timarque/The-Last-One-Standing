cmake_minimum_required(VERSION 3.10)

project(VR_project)

find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)

set(SOURCES_MAIN "Project/sources/main.cpp" 
                 "Project/sources/Mesh.cpp" 
                 "Project/sources/Model.cpp" 
                 "Project/sources/PhysicModel.cpp"
                 "Project/sources/TankModel.cpp"
                 "Project/sources/debugObject.cpp")

add_compile_definitions(PATH_TO_OBJECTS="${CMAKE_CURRENT_SOURCE_DIR}/Project/objects")
add_compile_definitions(PATH_TO_SHADERS="${CMAKE_CURRENT_SOURCE_DIR}/Project/shaders")
add_compile_definitions(PATH_TO_TEXTURES="${CMAKE_CURRENT_SOURCE_DIR}/Project/textures")

add_executable(VR_project ${SOURCES_MAIN})

target_link_libraries(VR_project PRIVATE assimp glfw glad::glad ${BULLET_LIBRARIES})
