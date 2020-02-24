#include "oylpch.h"
#include "Model.h"
#include "Mesh.h"

#include "Utils/AssetCache.h"

static glm::mat4 aiToGlm(const aiMatrix4x4& v)
{
    return {
        v.a1, v.b1, v.c1, v.d1,
        v.a2, v.b2, v.c2, v.d2,
        v.a3, v.b3, v.c3, v.d3,
        v.a4, v.b4, v.c4, v.d4,
    };
}

namespace oyl
{
    const char* internal::AssetCache<Model>::s_typename = "Model";

    internal::AssetCache<Model> Model::s_cache = {};
    
    Model::Model(_Model, const std::string& filepath)
        : m_importer(new Assimp::Importer)
    {
        loadFromFile(filepath);
    }

    Model::~Model()
    {
        m_scene = nullptr;
        delete m_importer;
    }

    void Model::loadFromFile(const std::string& filepath)
    {
        if (!m_meshes.empty())
            m_meshes.clear();

        if (m_scene)
        {
            m_importer->FreeScene();
            m_scene = nullptr;
        }

        const auto flags = (aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GlobalScale);

        m_scene = m_importer->ReadFile(filepath.c_str(), flags);
        
        if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
        {
            OYL_LOG_ERROR("Assimp Error: {}", m_importer->GetErrorString());
            return;
        }

        if (m_scene->mMetaData)
        {
            double tempInputScale;
            if (m_scene->mMetaData->Get("UnitScaleFactor", tempInputScale))
                m_unitScale = static_cast<float>(tempInputScale) * 0.01f;
        }

        m_globalInverseTransform = aiToGlm(m_scene->mRootNode->mTransformation);
        m_globalInverseTransform[3] *= m_unitScale;
        m_globalInverseTransform[3].w = 1.0f;
        m_globalInverseTransform = inverse(m_globalInverseTransform);
        //m_globalInverseTransform = inverse(aiToGlm(m_scene->mRootNode->mTransformation));

        m_filepath = filepath;
        
        if (m_scene->mNumMeshes > 0)
        {
            for (uint i = 0; i < m_scene->mNumMeshes; i++)
                m_meshes.push_back(processMesh(m_scene->mMeshes[i], m_scene));
        }

        if (m_scene->mNumAnimations > 0)
        {
            for (uint i = 0; i < m_scene->mNumAnimations; i++)
            {
                std::string name = m_scene->mAnimations[i]->mName.C_Str();
                m_animations[name] = m_scene->mAnimations[i];
            }
        }
    }

    Mesh Model::processMesh(aiMesh* a_mesh, const aiScene* a_scene)
    {
        std::vector<glm::vec3>  positions;      positions.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec2>  textureCoords;  textureCoords.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec3>  normals;        normals.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec3>  tangents;       tangents.reserve(a_mesh->mNumVertices);
        std::vector<glm::vec3>  bitangents;     bitangents.reserve(a_mesh->mNumVertices);
        std::vector<glm::ivec4> bones;          bones.resize(a_mesh->mNumVertices, glm::ivec4(-1));
        std::vector<glm::vec4>  boneWeights;    boneWeights.resize(a_mesh->mNumVertices);

        for (uint i = 0; i < a_mesh->mNumVertices; i++)
        {
            glm::vec3 temp;
            if (a_mesh->HasPositions())
            {
                temp.x = a_mesh->mVertices[i].x, temp.y = a_mesh->mVertices[i].y, temp.z = a_mesh->mVertices[i].z;
                positions.push_back(temp * m_unitScale);
                //positions.push_back(temp);
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
        if (a_mesh->HasBones())
        {
            for (uint i = 0; i < a_mesh->mNumBones; i++)
            {
                uint boneIndex = 0;
                std::string boneName(a_mesh->mBones[i]->mName.C_Str());

                if (auto it = m_boneIndices.find(boneName); it == m_boneIndices.end())
                {
                    boneIndex = m_numBones;
                    m_numBones++;
                    m_boneInfos.emplace_back();
                }
                else boneIndex = it->second;

                m_boneIndices[boneName] = boneIndex;
                m_boneInfos[boneIndex].inverseTransform = aiToGlm(a_mesh->mBones[i]->mOffsetMatrix);
                m_boneInfos[boneIndex].inverseTransform[3] *= m_unitScale;
                m_boneInfos[boneIndex].inverseTransform[3].w = 1.0f;
                
                //m_boneInfos[boneIndex].inverseTransform = aiToGlm(a_mesh->mBones[i]->mOffsetMatrix);

                for (uint j = 0; j < a_mesh->mBones[i]->mNumWeights; j++)
                {
                    uint vertexID = a_mesh->mBones[i]->mWeights[j].mVertexId;
                    float weight  = a_mesh->mBones[i]->mWeights[j].mWeight;
                    for (uint n = 0; n < 4; n++)
                    {
                        if (bones[vertexID][n] == -1)
                        {
                            bones[vertexID][n] = boneIndex;
                            boneWeights[vertexID][n] = weight;
                            break;
                        }
                    }
                }
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

                if (a_mesh->HasBones())
                {
                    data.bones = bones[face.mIndices[j]];
                    data.boneWeights = boneWeights[face.mIndices[j]];
                }

                unpackedData.push_back(data);
            }
        }

        return Mesh(unpackedData.data(), unpackedData.size());
    }

    void Model::getBoneTransforms(const std::string& animation, float time, std::vector<glm::mat4>& transforms)
    {
        if (auto it = m_animations.find(animation); it != m_animations.end() && it->second)
        {
            glm::mat4 identity = glm::mat4(1.0f);

            const aiAnimation* anim = it->second;

            float ticksPerSecond = anim->mTicksPerSecond != 0
                                       ? anim->mTicksPerSecond
                                       : 30.0f;

            float timeInTicks = time * ticksPerSecond;
            float animTime = std::fmod(timeInTicks, static_cast<float>(anim->mDuration) - 0.001f);

            readNodeHierarchy(animation, animTime, m_scene->mRootNode, identity);
            
            transforms.resize(m_numBones);

            for (uint i = 0; i < m_numBones; i++)
            {
                transforms[i] = m_boneInfos[i].finalTransform;
            }
        }
    }

    void Model::readNodeHierarchy(const std::string& animation, float a_animTime, const aiNode* a_node, glm::mat4 a_parentTransform)
    {
        const aiAnimation* anim = m_animations[animation];
        
        std::string nodeName = a_node->mName.C_Str();

        glm::mat4 nodeTransform = aiToGlm(a_node->mTransformation);
        nodeTransform[3] *= m_unitScale;
        nodeTransform[3].w = 1.0f;
        //glm::mat4 nodeTransform = aiToGlm(a_node->mTransformation);

        const aiNodeAnim* nodeAnim = nullptr;
        for (uint i = 0; i < anim->mNumChannels; i++)
        {
            if (anim->mChannels[i]->mNodeName.C_Str() == nodeName)
                nodeAnim = anim->mChannels[i];
        }

        if (nodeAnim)
        {
            glm::vec3 position = calcInterpolatedPosition(a_animTime, nodeAnim);
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);

            glm::quat rotation = calcInterpolatedRotation(a_animTime, nodeAnim);

            glm::vec3 scaling = calcInterpolatedScale(a_animTime, nodeAnim);
            glm::mat4 scaleMat = scale(glm::mat4(1.0f), scaling);

            nodeTransform = translationMat * mat4_cast(rotation) * scaleMat;
        }

        glm::mat4 globalTransform = a_parentTransform * nodeTransform;

        if (auto it = m_boneIndices.find(nodeName); it != m_boneIndices.end())
        {
            uint boneIndex = it->second;
            m_boneInfos[boneIndex].finalTransform = m_globalInverseTransform * globalTransform * m_boneInfos[boneIndex].inverseTransform;
            //m_boneInfos[boneIndex].finalTransform *= glm::scale(glm::mat4(1.0f), glm::vec3(m_unitScale));
        }

        for (uint i = 0; i < a_node->mNumChildren; i++)
            readNodeHierarchy(animation, a_animTime, a_node->mChildren[i], globalTransform);
    }

    glm::vec3 Model::calcInterpolatedPosition(float a_animTime, const aiNodeAnim* a_nodeAnim)
    {
        if (a_nodeAnim->mNumPositionKeys == 0)
            return glm::vec3(0.0f);
        if (a_nodeAnim->mNumPositionKeys == 1)
        {
            const auto& val = a_nodeAnim->mPositionKeys[0].mValue;
            return glm::vec3(val.x, val.y, val.z) * m_unitScale;
            //return glm::vec3(val.x, val.y, val.z);
        }

        uint positionIndex = 0;
        for (uint i = 0; i < a_nodeAnim->mNumPositionKeys; i++)
            if (a_animTime < a_nodeAnim->mPositionKeys[i + 1].mTime)
            {
                positionIndex = i;
                break;
            }

        uint nextPositionIndex = positionIndex + 1;

        float deltaTime = static_cast<float>(a_nodeAnim->mPositionKeys[nextPositionIndex].mTime -
                                             a_nodeAnim->mPositionKeys[positionIndex].mTime);
        
        float factor = (a_animTime - static_cast<float>(a_nodeAnim->mPositionKeys[positionIndex].mTime)) / deltaTime;
        factor = glm::clamp(factor, 0.0f, 1.0f);

        const auto& aiStart = a_nodeAnim->mPositionKeys[positionIndex].mValue;
        const auto& aiEnd = a_nodeAnim->mPositionKeys[nextPositionIndex].mValue;

        glm::vec3 start = { aiStart.x, aiStart.y, aiStart.z };
        glm::vec3 end = { aiEnd.x, aiEnd.y, aiEnd.z };
        return glm::mix(start, end, factor) * m_unitScale;
        //return glm::mix(start, end, factor);
    }

    glm::quat Model::calcInterpolatedRotation(float a_animTime, const aiNodeAnim* a_nodeAnim)
    {
        if (a_nodeAnim->mNumRotationKeys == 0)
            return glm::vec3(0.0f);
        if (a_nodeAnim->mNumRotationKeys == 1)
        {
            auto val = a_nodeAnim->mRotationKeys[0].mValue;
            return { val.w, val.x, val.y, val.z };
        }

        uint rotationIndex = 0;
        for (uint i = 0; i < a_nodeAnim->mNumRotationKeys - 1; i++)
            if (a_animTime < a_nodeAnim->mRotationKeys[i + 1].mTime)
            {
                rotationIndex = i;
                break;
            }

        uint nextRotationIndex = rotationIndex + 1;

        float deltaTime = static_cast<float>(a_nodeAnim->mRotationKeys[nextRotationIndex].mTime -
                                             a_nodeAnim->mRotationKeys[rotationIndex].mTime);

        float factor = (a_animTime - static_cast<float>(a_nodeAnim->mRotationKeys[rotationIndex].mTime)) / deltaTime;
        factor = glm::clamp(factor, 0.0f, 1.0f);
        
        const auto& aiStart = a_nodeAnim->mRotationKeys[rotationIndex].mValue;
        const auto& aiEnd = a_nodeAnim->mRotationKeys[nextRotationIndex].mValue;

        glm::quat start = { aiStart.w, aiStart.x, aiStart.y, aiStart.z };
        glm::quat end = { aiEnd.w, aiEnd.x, aiEnd.y, aiEnd.z };
        return normalize(slerp(start, end, factor));
    }

    glm::vec3 Model::calcInterpolatedScale(float a_animTime, const aiNodeAnim* a_nodeAnim)
    {
        if (a_nodeAnim->mNumScalingKeys == 0)
            return glm::vec3(0.0f);
        if (a_nodeAnim->mNumScalingKeys == 1)
        {
            auto val = a_nodeAnim->mScalingKeys[0].mValue;
            return { val.x, val.y, val.z };
        }

        uint scalingIndex = 0;
        for (uint i = 0; i < a_nodeAnim->mNumScalingKeys - 1; i++)
            if (a_animTime < a_nodeAnim->mScalingKeys[i + 1].mTime)
            {
                scalingIndex = i;
                break;
            }

        uint nextScalingIndex = scalingIndex + 1;

        float deltaTime = static_cast<float>(a_nodeAnim->mScalingKeys[nextScalingIndex].mTime -
                                             a_nodeAnim->mScalingKeys[scalingIndex].mTime);

        float factor = (a_animTime - static_cast<float>(a_nodeAnim->mScalingKeys[scalingIndex].mTime)) / deltaTime;
        factor = glm::clamp(factor, 0.0f, 1.0f);

        const auto& aiStart = a_nodeAnim->mScalingKeys[scalingIndex].mValue;
        const auto& aiEnd = a_nodeAnim->mScalingKeys[nextScalingIndex].mValue;

        glm::vec3 start = { aiStart.x, aiStart.y, aiStart.z };
        glm::vec3 end = { aiEnd.x, aiEnd.y, aiEnd.z };
        return glm::mix(start, end, factor);
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
