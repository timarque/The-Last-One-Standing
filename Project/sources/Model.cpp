//
// Created by hanno on 27/12/2023.
//

#include "Model.h"
#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

unsigned int TextureFromFile(const char *path, const std::string &directory) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Model::Model(std::string path) {
    loadModel(path);
}

void Model::Draw(Shader &shader) {
    for(unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader, 0);
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
   
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.Normal = normal;

        glm::vec2 texCoords;
        if (mesh->mTextureCoords[0]) {  // if the mesh contains texture coordinates
            texCoords.x = mesh->mTextureCoords[0][i].x;
            texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertex.TexCoords = texCoords;

        
        glm::vec3 tangents;
        tangents.x = mesh->mTangents[i].x;
        tangents.y = mesh->mTangents[i].y;
        tangents.z = mesh->mTangents[i].z;
        vertex.Tangents = tangents;
        vertex.Bitangent = glm::normalize(glm::cross(vertex.Normal, vertex.Tangents));

        vertices.push_back(vertex);
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // le probleme vient probablement du fait que y a pas la texture normal dans material, je dois load la texture autrement que avec material du coup je pense
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {   // if it's the first time we encounter this texture
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

void Model::createPhysicsObject(btDiscreteDynamicsWorld *dynamicsWorld, btCollisionShape* collision_shape, float mass, btVector3 origin)
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(origin);

    btVector3 localInertia(1, 0, 0);
    collision_shape->calculateLocalInertia(mass, localInertia);

    // Ajuster l'inertie selon vos besoins

    btDefaultMotionState *motionState = new btDefaultMotionState(transform);
    
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collision_shape);
    physicsObject = new btRigidBody(rbInfo);
    physicsObject->setRestitution(0.8);
    physicsObject->setFriction(0.5f);
    physicsObject->applyForce(btVector3(0, 0, -10), btVector3(1, 0, 0));
    dynamicsWorld->addRigidBody(physicsObject);
}

// Mettre à jour la transformation du modèle à partir de la physique
void Model::updateFromPhysics()
{
    btTransform transform;
    physicsObject->getMotionState()->getWorldTransform(transform);
    btVector3 position = transform.getOrigin();
    btQuaternion rotation = transform.getRotation();
    for (Mesh mesh: meshes) mesh.setTransform(position.x(), position.y(), position.z(), rotation.w(), rotation.x(), rotation.y(), rotation.z());
}

void Model::addShader(Shader &shader)
{
    shaders.push_back(&shader);
}

void Model::DrawWithShader(Shader &shader, int apply)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader, apply);
    }
}

void Model::Draw()
{
    // Dessiner le modèle avec tous les shaders associés
    for (Shader *shader : shaders)
    {
        DrawWithShader(*shader, 0);
    }
}