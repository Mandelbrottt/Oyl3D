#include "oylpch.h"
#include "Model.h"
#include "Mesh.h"

#include "Utils/AssetCache.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace oyl
{
    const char* internal::AssetCache<Model>::s_typename = "Model";

    internal::AssetCache<Model> Model::s_cache = {};
    
    Model::Model(_Model, const std::string& filepath)
    {
        loadFromFile(filepath);
    }
    
    void Model::loadFromFile(const std::string& filepath)
    {
        if (!m_meshes.empty())
            m_meshes.clear();

        auto flags = (aiProcess_Triangulate | aiProcess_CalcTangentSpace);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath.c_str(), flags);
        //const aiScene* scene = nullptr;

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            OYL_LOG_ERROR("Assimp Error: {}", importer.GetErrorString());
            return;
        }

        if (scene->mRootNode->mNumMeshes > 0)
            processNode(scene->mRootNode, scene);
        else if (scene->mNumMeshes > 0)
        {
            for (uint i = 0; i < scene->mNumMeshes; i++)
                m_meshes.push_back(processMesh(scene->mMeshes[i], scene));
        }
    }

    void Model::processNode(aiNode* a_node, const aiScene* a_scene)
    {
        // Process all (if any) of the node's meshes
        for (uint i = 0; i < a_node->mNumMeshes; i++)
        {
            auto* meshPtr = a_scene->mMeshes[a_node->mMeshes[i]];
            Mesh tempMesh = processMesh(meshPtr, a_scene);
            if (tempMesh.getVertexArray())
                m_meshes.push_back(std::move(tempMesh));
        }

        // Process all (if any) of the node's children
        for (uint i = 0; i < a_node->mNumChildren; i++)
        {
            processNode(a_node->mChildren[i], a_scene);
        }
    }

    Mesh Model::processMesh(aiMesh* a_mesh, const aiScene* a_scene)
    {
        std::vector<glm::vec3> positions;     positions.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec2> textureCoords; textureCoords.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec3> normals;       normals.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec3> tangents;      tangents.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec3> bitangents;    bitangents.reserve(a_mesh->mNumVertices);

        for (uint i = 0; i < a_mesh->mNumVertices; i++)
        {
            glm::vec3 temp;
            if (a_mesh->HasPositions())
            {
                temp.x = a_mesh->mVertices[i].x, temp.y = a_mesh->mVertices[i].y, temp.z = a_mesh->mVertices[i].z;
                positions.push_back(temp);
            }
            if (a_mesh->HasTextureCoords(0))
            {
                temp.x = a_mesh->mTextureCoords[0][i].x, temp.y = a_mesh->mTextureCoords[0][i].y;
                textureCoords.push_back(temp.xy);
            }
            if (a_mesh->HasNormals())
            {
                temp.x = a_mesh->mNormals[i].x, temp.y = a_mesh->mNormals[i].y, temp.z = a_mesh->mNormals[i].z;
                normals.push_back(temp);
            }
            if (a_mesh->HasTangentsAndBitangents())
            {
                temp.x = a_mesh->mTangents[i].x, temp.y = a_mesh->mTangents[i].y, temp.z = a_mesh->mTangents[i].z;
                tangents.push_back(temp);

                temp.x = a_mesh->mBitangents[i].x, temp.y = a_mesh->mBitangents[i].y, temp.z = a_mesh->mBitangents[i].z;
                bitangents.push_back(temp);

                // Calculating tangents and bitangents manually if necessary
                //for (size_t i = 0; i < unpackedData.size(); i += 3)
                //{
                //    BufferData* data = &unpackedData[i];

                //    glm::vec3 edge1 = data[1].position - data[0].position;
                //    glm::vec3 edge2 = data[2].position - data[0].position;
                //    glm::vec2 deltaUV1 = data[1].UVs - data[0].UVs;
                //    glm::vec2 deltaUV2 = data[2].UVs - data[0].UVs;

                //    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

                //    glm::vec3 tangent = {};
                //    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                //    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                //    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                //    tangent = glm::normalize(tangent);

                //    glm::vec3 bitangent = {};
                //    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                //    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                //    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                //    bitangent = glm::normalize(bitangent);

                //    for (int j = 0; j < 3; j++)
                //        data[j].tangent = tangent, data[j].biTangent = bitangent;
                //}
            }
        }

        std::vector<VertexData> unpackedData;
        unpackedData.reserve(a_mesh->mNumFaces * 3);

        for (uint i = 0; i < a_mesh->mNumFaces; i++)
        {
            aiFace face = a_mesh->mFaces[i];
            for (uint j = 0; j < face.mNumIndices; j++)
            {
                VertexData data{};

                if (a_mesh->HasPositions())
                    data.position = positions[face.mIndices[j]];

                if (a_mesh->HasTextureCoords(0))
                    data.textureCoords = textureCoords[face.mIndices[j]];

                if (a_mesh->HasNormals())
                    data.normal = normals[face.mIndices[j]];

                if (a_mesh->HasTangentsAndBitangents())
                {
                    data.tangent = tangents[face.mIndices[j]];
                    data.biTangent = bitangents[face.mIndices[j]];
                }

                unpackedData.push_back(data);
            }
        }

        return Mesh(unpackedData.data(), unpackedData.size());
    }

    void Model::init()
    {
        Model::cache(ENGINE_RES + INVALID_MODEL_PATH, INVALID_ALIAS);
    }

    Ref<Model> Model::create(const std::string& filepath)
    {
        return Ref<Model>::create(_Model{}, filepath);
    }
    
    const Ref<Model>& Model::cache(const std::string& filepath, const CacheAlias& alias, bool overwrite)
    {
        return s_cache.cache(filepath, alias, overwrite);
    }
    
    const Ref<Model>& Model::cache(const Ref<Model>& existing, const CacheAlias& alias, bool overwrite)
    {
        return s_cache.cache(existing, alias, overwrite);
    }

    void Model::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }
    
    const Ref<Model>& Model::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    bool Model::isCached(const Ref<Model>& existing)
    {
        return s_cache.isCached(existing);
    }

    bool Model::exists(const CacheAlias& alias)
    {
        return s_cache.exists(alias);
    }

    const CacheAlias& Model::getAlias(const Ref<Model>& existing)
    {
        return s_cache.getAlias(existing);
    }

    const Ref<Model>& Model::rename(const CacheAlias& currentAlias, const CacheAlias& newAlias, bool overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }
}
