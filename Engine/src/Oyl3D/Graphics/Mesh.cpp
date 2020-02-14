#include "oylpch.h"
#include "Mesh.h"

#include "Graphics/Buffer.h"

#include "Rendering/Renderer.h"

#include "Utils/AssetCache.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace oyl
{
    //Ref<Mesh> Mesh::s_invalid;

    //std::unordered_map<std::string, Ref<Mesh>> Mesh::s_cache;

    //internal::AssetCache<Mesh> Mesh::s_cache;

    //const char* internal::AssetCache<Mesh>::s_typename = "Mesh";
    
    //Mesh::Mesh(_Mesh, const std::string& file)
    //{
    //    if (loadFromFile(filePath))
    //        m_filePath = filePath;
    //}
    
    Mesh::Mesh(VertexData* a_vertices, uint a_numVertices)
    {
        loadFromVertices(a_vertices, a_numVertices);
    }

    //bool Mesh::loadFromFile(const std::string& filepath)
    //{
    //    auto flags = (aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    //    Assimp::Importer importer;
    //    const aiScene* scene = importer.ReadFile(filepath, flags);

    //    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    //    {
    //        OYL_LOG_ERROR("Assimp Error: {}", importer.GetErrorString());
    //        return false;
    //    }

    //    aiMesh* mesh = scene->mMeshes[0];

    //    std::vector<glm::vec3> positions;     positions.reserve(mesh->mNumVertices);
    //    std::vector<glm::vec2> textureCoords; textureCoords.reserve(mesh->mNumVertices);
    //    std::vector<glm::vec3> normals;       normals.reserve(mesh->mNumVertices);
    //    std::vector<glm::vec3> tangents;      tangents.reserve(mesh->mNumVertices);
    //    std::vector<glm::vec3> bitangents;    bitangents.reserve(mesh->mNumVertices);

    //    for (uint i = 0; i < mesh->mNumVertices; i++)
    //    {
    //        glm::vec3 temp;
    //        if (mesh->HasPositions())
    //        {
    //            temp.x = mesh->mVertices[i].x, temp.y = mesh->mVertices[i].y, temp.z = mesh->mVertices[i].z;
    //            positions.push_back(temp);
    //        }
    //        if (mesh->HasTextureCoords(0))
    //        {
    //            temp.x = mesh->mTextureCoords[0][i].x, temp.y = mesh->mTextureCoords[0][i].y;
    //            textureCoords.push_back(temp.xy);
    //        }
    //        if (mesh->HasNormals())
    //        {
    //            temp.x = mesh->mNormals[i].x, temp.y = mesh->mNormals[i].y, temp.z = mesh->mNormals[i].z;
    //            normals.push_back(temp);
    //        }
    //        if (mesh->HasTangentsAndBitangents())
    //        {
    //            temp.x = mesh->mTangents[i].x, temp.y = mesh->mTangents[i].y, temp.z = mesh->mTangents[i].z;
    //            tangents.push_back(temp);

    //            temp.x = mesh->mBitangents[i].x, temp.y = mesh->mBitangents[i].y, temp.z = mesh->mBitangents[i].z;
    //            bitangents.push_back(temp);

    //            // Calculating tangents and bitangents manually if necessary
    //            //for (size_t i = 0; i < unpackedData.size(); i += 3)
    //            //{
    //            //    BufferData* data = &unpackedData[i];

    //            //    glm::vec3 edge1 = data[1].position - data[0].position;
    //            //    glm::vec3 edge2 = data[2].position - data[0].position;
    //            //    glm::vec2 deltaUV1 = data[1].UVs - data[0].UVs;
    //            //    glm::vec2 deltaUV2 = data[2].UVs - data[0].UVs;

    //            //    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    //            //    glm::vec3 tangent = {};
    //            //    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    //            //    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    //            //    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    //            //    tangent = glm::normalize(tangent);

    //            //    glm::vec3 bitangent = {};
    //            //    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    //            //    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    //            //    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    //            //    bitangent = glm::normalize(bitangent);

    //            //    for (int j = 0; j < 3; j++)
    //            //        data[j].tangent = tangent, data[j].biTangent = bitangent;
    //            //}
    //        }
    //    }
    //    
    //    std::vector<VertexData> unpackedData;
    //    unpackedData.reserve(mesh->mNumFaces * 3);

    //    for (uint i = 0; i < mesh->mNumFaces; i++)
    //    {
    //        aiFace face = mesh->mFaces[i];
    //        for (uint j = 0; j < face.mNumIndices; j++)
    //        {
    //            VertexData data{};
    //            
    //            if (mesh->HasPositions())
    //                data.position = positions[face.mIndices[j]];

    //            if (mesh->HasTextureCoords(0))
    //                data.textureCoords = textureCoords[face.mIndices[j]];

    //            if (mesh->HasNormals())
    //                data.normal = normals[face.mIndices[j]];

    //            if (mesh->HasTangentsAndBitangents())
    //            {   
    //                data.tangent   = tangents[face.mIndices[j]];
    //                data.biTangent = bitangents[face.mIndices[j]];
    //            }

    //            unpackedData.push_back(data);
    //        }
    //    }
    //    
    //    return true;
    //}

    void Mesh::loadFromVertices(VertexData* a_vertices, uint a_numVertices)
    {
        if (!a_vertices || a_numVertices == 0)
            return;
        
        m_vbo = VertexBuffer::create(reinterpret_cast<float*>(a_vertices), a_numVertices * sizeof(VertexData));

        BufferLayout layout = {
            { DataType::Float3, "in_position" },
            { DataType::Float2, "in_textureUV" },
            { DataType::Float3, "in_normal" },
            { DataType::Float3, "in_tangent" },
            { DataType::Float3, "in_biTangent" },
        };

        m_vbo->setLayout(layout);

        m_vao = VertexArray::create();
        m_vao->addVertexBuffer(m_vbo);

        m_vbo->unbind();
        m_vao->unbind();
        
        m_numVertices = a_numVertices;
        m_numFaces = a_numVertices / 3;
    }

    void Mesh::unload()
    {
        m_vbo.reset();
        m_vao.reset();
    }

    void Mesh::bind()
    {
        m_vao->bind();
    }

    void Mesh::unbind()
    {
        m_vao->unbind();
    }

    //Ref<Mesh> Mesh::create(VertexData* a_vertices, uint a_numVertices)
    //{
    //    Ref<Mesh> mesh = nullptr;
    //    if (a_vertices && a_numVertices > 0)
    //    {
    //        mesh = Ref<Mesh>::create(_Mesh{}, a_vertices, a_numVertices);
    //        if (mesh->m_numVertices <= 0)
    //            mesh = nullptr;
    //    }
    //    if (mesh == nullptr)
    //        mesh = s_cache.get(INVALID_ALIAS);

    //    return mesh;
    //}

    ////Ref<Mesh> Mesh::create(const std::string& filePath)
    ////{
    ////    auto mesh = Ref<Mesh>::create(_Mesh{}, filePath);
    ////    return mesh->m_numVertices <= 0 ? s_cache.get(INVALID_ALIAS) : mesh;
    ////}

    //const Ref<Mesh>& Mesh::cache(VertexData* a_vertices, 
    //                             uint a_numVertices, 
    //                             const CacheAlias& alias, 
    //                             bool overwrite)
    //{
    //    auto mesh = Mesh::create(a_vertices, a_numVertices);
    //    return s_cache.cache(mesh, alias, overwrite);
    //}

    //const Ref<Mesh>& Mesh::cache(const Ref<Mesh>&  existing,
    //                             const CacheAlias& alias,
    //                             bool overwrite)
    //{
    //    return s_cache.cache(existing, alias, overwrite);
    //}

    //void Mesh::discard(const CacheAlias& alias)
    //{
    //    s_cache.discard(alias);
    //}

    //const Ref<Mesh>& Mesh::get(const CacheAlias& alias)
    //{
    //    return s_cache.get(alias);
    //}

    //bool Mesh::isCached(const Ref<Mesh>& existing)
    //{
    //    return s_cache.isCached(existing);
    //}

    //bool Mesh::exists(const CacheAlias& alias)
    //{
    //    return s_cache.exists(alias);
    //}

    //const CacheAlias& Mesh::getAlias(const Ref<Mesh>& existing)
    //{
    //    return s_cache.getAlias(existing);
    //}

    //const Ref<Mesh>& Mesh::rename(const CacheAlias& currentAlias,
    //                              const CacheAlias& newAlias,
    //                              bool              overwrite)
    //{
    //    return s_cache.rename(currentAlias, newAlias, overwrite);
    //}

    //void Mesh::init()
    //{
    //    Mesh::cache(ENGINE_RES + INVALID_MESH_PATH, INVALID_ALIAS);
    //}
}
