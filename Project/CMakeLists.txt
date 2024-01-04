project("project")
# Changer le nom une fois qu'on en aura un

set(SOURCES_MAIN 
    "sources/main.cpp" 
    "sources/Mesh.h" 
    "sources/Mesh.cpp" 
    "sources/Object.h" 
    "sources/Model.h" 
    "sources/Model.cpp" 
    "sources/PhysicModel.h" 
    "sources/PhysicModel.cpp" 
    "sources/TankModel.hpp" 
    "sources/TankModel.cpp" 
    "sources/debugObject.hpp"
    "sources/debugObject.cpp" 
)


#These commands are there to specify the path to the folder containing the object and textures files as macro
#With these you can just use PATH_TO_OBJECTS and PATH_TO_TEXTURE in your c++ code and the compiler will replace it by the correct expression
add_compile_definitions(PATH_TO_OBJECTS="${CMAKE_CURRENT_SOURCE_DIR}/objects")
add_compile_definitions(PATH_TO_TEXTURES="${CMAKE_CURRENT_SOURCE_DIR}/textures")
add_compile_definitions(PATH_TO_SHADERS="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

add_executable(${PROJECT_NAME}_main ${SOURCES_MAIN})
target_link_libraries(${PROJECT_NAME}_main PUBLIC OpenGL::GL glfw glad assimp BulletDynamics BulletCollision LinearMath)
