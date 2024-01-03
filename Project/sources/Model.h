//
// Created by hanno on 27/12/2023.
//

#ifndef VR_PROJECT_MODEL_H
#define VR_PROJECT_MODEL_H

#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION

#include <vector>
#include <iostream>
#include "Shader.h"
#include "Mesh.h"
#include <map>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "bullet/btBulletDynamicsCommon.h"

unsigned int TextureFromFile(const char *path, const std::string &directory);

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;

};


/**
 * Model class that loads a model from a file
 */

class Model {

public:
    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }
    /**
     * Constructor of a Model
     * @param path to the model file
     */
    Model(std::string path);

    /**
     * Draws the model in the 3D scene
     * @param shader to use to draw the model
     */
    void Draw(Shader &shader);
    
    // Ajouter un shader au modèle
    void addShader(Shader &shader);

    // Dessiner le modèle avec un shader spécifié
    void DrawWithShader(Shader &shader, int apply);
    void Draw();

    std::vector<Texture> textures_loaded;   // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

protected:
    // Model datas
    std::string directory;
    std::vector<Mesh> meshes;
    std::vector<Shader *> shaders;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

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

    void SetVertexBoneDataToDefault(Vertex& vertex);
    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

};


#endif //VR_PROJECT_MODEL_H
