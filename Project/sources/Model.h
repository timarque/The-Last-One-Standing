//
// Created by hanno on 27/12/2023.
//

#ifndef VR_PROJECT_MODEL_H
#define VR_PROJECT_MODEL_H

#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

#include <vector>
#include "Shader.h"
#include "Mesh.h"

#include "../../3rdParty/assimp/include/assimp/Importer.hpp"
#include "../../3rdParty/assimp/include/assimp/scene.h"
#include "../../3rdParty/assimp/include/assimp/postprocess.h"

unsigned int TextureFromFile(const char *path, const std::string &directory);


/**
 * Model class that loads a model from a file
 */
class Model {

public:
    /**
     * Constructor of a Model
     * @param path to the model file
     */
    Model(const char *path);

    /**
     * Draws the model in the 3D scene
     * @param shader to use to draw the model
     */
    void Draw(Shader &shader);

private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;   // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

    /**
     * Loads a model from a file
     * @param path to the model file
     */
    void loadModel(std::string path);

    /**
     * Processes a node of the model
     * @param node to process
     * @param scene the node belongs to
     */
    void processNode(aiNode *node, const aiScene *scene);

    /**
     * Processes a mesh of the model
     * @param mesh to process
     * @param scene the mesh belongs to
     * @return the processed mesh
     */
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    /**
     * Loads the textures of a material
     * @param mat the material
     * @param type of the texture
     * @param typeName of the texture
     * @return the loaded textures
     */
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};


#endif //VR_PROJECT_MODEL_H
