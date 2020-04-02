#include "oylpch.h"
#include "Model.h"
#include "Mesh.h"

#include "Utils/AssetCache.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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
    {
        loadFromFile(filepath);
    }
    
    void Model::loadFromFile(const std::string& filepath)
    {
        if (!m_meshes.empty())
            m_meshes.clear();

        auto flags = (aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_GlobalScale);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath.c_str(), flags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            OYL_LOG_ERROR("Assimp Error: {}", importer.GetErrorString());
            return;
        }
        if (scene->mMetaData)
        {
            double tempInputScale;
            if (scene->mMetaData->Get("UnitScaleFactor", tempInputScale))
                m_unitScale = static_cast<float>(tempInputScale) * 0.01f;
        }

        m_filepath = filepath;

        auto aiT = scene->mRootNode->mTransformation;
        m_globalInverseTransform = inverse(aiToGlm(aiT));
        m_globalInverseTransform[3].xyz *= glm::vec3(m_unitScale);

        if (scene->mNumMeshes > 0)
        {
            for (uint i = 0; i < scene->mNumMeshes; i++)
                m_meshes.push_back(processMesh(scene->mMeshes[i], scene));
        }

        if (scene->mRootNode && !m_bones.empty())
            processNode(scene->mRootNode);

        if (scene->mNumAnimations > 0)
        {
            m_animations.reserve(scene->mNumAnimations);
            for (uint i = 0; i < scene->mNumAnimations; i++)
            {
                processAnimation(scene->mAnimations[i]);
            }
        }
    }

    Mesh Model::processMesh(aiMesh* a_mesh, const aiScene* a_scene)
    {
        std::vector<glm::vec3>  positions;     
        std::vector<glm::vec2>  textureCoords; 
        std::vector<glm::vec3>  normals;       
        std::vector<glm::vec3>  tangents;      
        std::vector<glm::vec3>  bitangents;    
        std::vector<glm::ivec4> bones;         
        std::vector<glm::vec4>  boneWeights;

        // Reserve the memory necessary for all of the vertices ahead of time
        if (a_mesh->mNumVertices > 0)
        {
            positions.reserve(a_mesh->mNumVertices);
            textureCoords.reserve(a_mesh->mNumVertices);
            normals.reserve(a_mesh->mNumVertices);
            tangents.reserve(a_mesh->mNumVertices);
            bitangents.reserve(a_mesh->mNumVertices);
            bones.assign(a_mesh->mNumVertices, glm::ivec4(-1));
            boneWeights.assign(a_mesh->mNumVertices, glm::vec4(0.0f));
        }
        
        for (uint i = 0; i < a_mesh->mNumVertices; i++)
        {
            glm::vec4 temp = glm::vec4(0.0f);
            if (a_mesh->HasPositions())
            {
                temp.x = a_mesh->mVertices[i].x, temp.y = a_mesh->mVertices[i].y, temp.z = a_mesh->mVertices[i].z;
                positions.push_back(temp.xyz * m_unitScale);
            }
            if (a_mesh->HasTextureCoords(0))
            {
                temp.x = a_mesh->mTextureCoords[0][i].x, temp.y = a_mesh->mTextureCoords[0][i].y;
                textureCoords.push_back(temp.xy);
            }
            if (a_mesh->HasNormals())
            {
                temp.x = a_mesh->mNormals[i].x, temp.y = a_mesh->mNormals[i].y, temp.z = a_mesh->mNormals[i].z;
                normals.push_back(temp.xyz);
            }
            if (a_mesh->HasTangentsAndBitangents())
            {
                temp.x = a_mesh->mTangents[i].x, temp.y = a_mesh->mTangents[i].y, temp.z = a_mesh->mTangents[i].z;
                tangents.push_back(temp.xyz);

                temp.x = a_mesh->mBitangents[i].x, temp.y = a_mesh->mBitangents[i].y, temp.z = a_mesh->mBitangents[i].z;
                bitangents.push_back(temp.xyz);

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

        m_bones.reserve(a_mesh->mNumBones);

        // Reconstruct the node hierarchy from Assimp
        for (uint i = 0; i < a_mesh->mNumBones; i++)
        {
            uint boneIndex = 0;
            std::string boneName(a_mesh->mBones[i]->mName.C_Str());

            if (auto it = m_boneIDs.find(boneName); it == m_boneIDs.end())
            {
                boneIndex = m_numMeshBones;
                m_bones.emplace_back();
                m_numMeshBones++;
            }
            else boneIndex = it->second;

            m_boneIDs[boneName] = boneIndex;
            m_bones[boneIndex].inverseTransform = aiToGlm(a_mesh->mBones[i]->mOffsetMatrix);
            m_bones[boneIndex].inverseTransform[3].xyz *= glm::vec3(m_unitScale);

            for (uint j = 0; j < a_mesh->mBones[i]->mNumWeights; j++)
            {
                uint vertexID = a_mesh->mBones[i]->mWeights[j].mVertexId;
                float weight  = a_mesh->mBones[i]->mWeights[j].mWeight;
                for (int n = 0; n < 4; n++)
                {
                    if (bones[vertexID][n] == -1)
                    {
                        bones[vertexID][n]       = boneIndex;
                        boneWeights[vertexID][n] = weight;
                        break;
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

    void Model::processNode(aiNode* a_node)
    {
        std::string name(a_node->mName.C_Str());
        auto it = m_boneIDs.find(name);
        if (it == m_boneIDs.end())
        {
            auto pair = m_boneIDs.emplace(name, m_bones.size());

            if (pair.second)
                it = pair.first;
            else
            {
                OYL_LOG_ERROR("Could not build the node graph for model {}!", m_filepath);
                return;
            }
            
            it->second = m_bones.size();
            m_bones.emplace_back();
        }
        
        auto& bone = m_bones[it->second];
        bone.transform = aiToGlm(a_node->mTransformation);
        bone.transform[3].xyz *= glm::vec3(m_unitScale);
        
        if (a_node->mParent)
        {
            std::string parentName(a_node->mParent->mName.C_Str());
            if (auto pIt = m_boneIDs.find(parentName); pIt != m_boneIDs.end())
            {
                bone.parent = pIt->second;
                m_bones[bone.parent].children.push_back(it->second);
            }
        }
        
        std::string childName;
        for (uint i = 0; i < a_node->mNumChildren; i++)
        {
            //childName.assign(a_node->mChildren[i]->mName.C_Str());
            //if (auto cIt = m_boneIDs.find(childName); cIt != m_boneIDs.end())
            //    bone.children.push_back(cIt->second);
        
            processNode(a_node->mChildren[i]);
        }
        //else
        //{
        //    for (uint i = 0; i < a_node->mNumChildren; i++)
        //    {
        //        processNode(a_node->mChildren[i]);
        //    }
        //}
    }

    void Model::processAnimation(aiAnimation* a_animation)
    {
        if (m_bones.empty())
            return;
        
        SkeletonAnimation sAnim = {};

        if (a_animation->mTicksPerSecond != 0.0)
            sAnim.tickRate = static_cast<float>(a_animation->mTicksPerSecond);

        float oneOverTickRate = 1.0f / sAnim.tickRate;
        
        sAnim.duration = static_cast<float>(a_animation->mDuration) * oneOverTickRate;

        sAnim.channels.reserve(a_animation->mNumChannels);

        for (uint i = 0; i < a_animation->mNumChannels; i++)
        {
            aiNodeAnim* channel = a_animation->mChannels[i];

            std::string nodeName(channel->mNodeName.C_Str());
            if (auto it = m_boneIDs.find(nodeName); it != m_boneIDs.end())
            {
                BoneChannel& bc = sAnim.channels[it->second];
                
                bc.positionKeys.reserve(channel->mNumPositionKeys);
                for (uint j = 0; j < channel->mNumPositionKeys; j++)
                {
                    auto pair   = std::make_pair<float, glm::vec3>(0.0f, {});
                    pair.first  = static_cast<float>(channel->mPositionKeys[j].mTime) * oneOverTickRate;
                    auto pos    = channel->mPositionKeys[j].mValue;
                    pair.second = { pos.x, pos.y, pos.z };
                    pair.second *= m_unitScale;
                    bc.positionKeys.push_back(pair);
                }

                bc.rotationKeys.reserve(channel->mNumRotationKeys);
                for (uint j = 0; j < channel->mNumRotationKeys; j++)
                {
                    auto pair   = std::make_pair<float, glm::quat>(0.0f, {});
                    pair.first  = static_cast<float>(channel->mRotationKeys[j].mTime) * oneOverTickRate;
                    auto pos    = channel->mRotationKeys[j].mValue;
                    pair.second = normalize(glm::quat(pos.w, pos.x, pos.y, pos.z));
                    bc.rotationKeys.push_back(pair);
                }

                bc.scaleKeys.reserve(channel->mNumScalingKeys);
                for (uint j = 0; j < channel->mNumScalingKeys; j++)
                {
                    auto pair   = std::make_pair<float, glm::vec3>(0.0f, {});
                    pair.first  = static_cast<float>(channel->mScalingKeys[j].mTime) * oneOverTickRate;
                    auto pos    = channel->mScalingKeys[j].mValue;
                    pair.second = { pos.x, pos.y, pos.z };
                    bc.scaleKeys.push_back(pair);
                }
            }
        }

        std::string animName(a_animation->mName.C_Str());
        std::string armatureName;
        
        uint pipePos = animName.find('|');
        if (pipePos != std::string::npos)
        {
            armatureName = animName.substr(0, pipePos);
            animName.erase(0, pipePos + 1);
        }

        if (m_animations.find(animName) != m_animations.end())
            OYL_LOG_ERROR("Duplicate animation '{}' found in armature '{}'!", animName, armatureName);
        else
            m_animations[animName] = std::move(sAnim);
    }

    void Model::getBoneTransforms(const std::string& animation, float time, std::vector<glm::mat4>& out) const
    {
        if (m_animations.find(animation) == m_animations.end())
            return;

        out.clear();
        out.reserve(m_bones.size());

        calculateFinalTransform(m_animations.at(animation), time, m_numMeshBones, glm::mat4(1.0f));

        for (const auto& bone : m_bones)
        {
            out.push_back(bone.finalTransform);
        }

        return;
    }

    glm::mat4 Model::getBoneTransform(const std::string& animation, const std::string& bone, float time) const
    {
        auto animIt = m_animations.find(animation);
        auto boneIt = m_boneIDs.find(bone);

        if (animIt == m_animations.end() || boneIt == m_boneIDs.end())
            return glm::mat4(1.0f);

        return _getBoneTransform(animIt->second, boneIt->second, time);
    }

    inline glm::mat4 Model::_getBoneTransform(const SkeletonAnimation& a_animation, uint a_bone, float a_time) const
    {
        if (a_bone > m_bones.size()) return glm::mat4(1.0f);
        
        glm::mat4 nodeTransform = m_bones[a_bone].transform;

        if (const auto it = a_animation.channels.find(a_bone); it != a_animation.channels.end())
        {
            const auto& animNode = it->second;

            glm::vec3 position = calcInterpolatedPosition(a_time, animNode, a_animation.lerpType, a_animation.lerpFn);
            nodeTransform = translate(glm::mat4(1.0f), position);

            glm::quat rotation = calcInterpolatedRotation(a_time, animNode, a_animation.lerpType, a_animation.lerpFn);
            nodeTransform *= mat4_cast(rotation);

            glm::vec3 scaling = calcInterpolatedScale(a_time, animNode, a_animation.lerpType, a_animation.lerpFn);
            nodeTransform = scale(nodeTransform, scaling);
        }

        return _getBoneTransform(a_animation, m_bones[a_bone].parent, a_time) * nodeTransform;
    }
    
    void Model::calculateFinalTransform(const SkeletonAnimation& animation,
                                        float                    time,
                                        uint                     bone,
                                        const glm::mat4&         parentTransform) const
    {
        glm::mat4 nodeTransform = m_bones[bone].transform;

        if (const auto it = animation.channels.find(bone); it != animation.channels.end())
        {
            const auto& animNode = it->second;
 
            glm::vec3 position = calcInterpolatedPosition(time, animNode, animation.lerpType, animation.lerpFn);
            nodeTransform = translate(glm::mat4(1.0f), position);
 
            glm::quat rotation = calcInterpolatedRotation(time, animNode, animation.lerpType, animation.lerpFn);
            nodeTransform *= mat4_cast(rotation);
 
            glm::vec3 scaling = calcInterpolatedScale(time, animNode, animation.lerpType, animation.lerpFn);
            nodeTransform = scale(nodeTransform, scaling);
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        if (bone < m_numMeshBones)
            m_bones[bone].finalTransform = m_globalInverseTransform * globalTransform * m_bones[bone].inverseTransform;

        for (uint b : m_bones[bone].children)
            calculateFinalTransform(animation, time, b, globalTransform);
    }

    glm::vec3 Model::calcInterpolatedPosition(float time, const BoneChannel& channel, 
                                              Interpolation::Type a_type, 
                                              Interpolation::EaseFn a_fn) const
    {
        if (channel.positionKeys.empty())
            return glm::vec3(0.0f);
        if (channel.positionKeys.size() == 1)
            return channel.positionKeys[0].second;

        uint positionIndex = [&]()
        {
            for (uint i = 0; i < channel.positionKeys.size() - 1; i++)
                if (time < channel.positionKeys[i + 1].first)
                    return i;
            return 0u;
        }();
        uint nextPositionIndex = positionIndex + 1;
        float deltaTime = channel.positionKeys[nextPositionIndex].first - channel.positionKeys[positionIndex].first;
        float factor = (time - channel.positionKeys[positionIndex].first) / deltaTime;

        if (a_fn) factor = a_fn(factor);
        
        glm::vec3 start = channel.positionKeys[positionIndex].second;
        glm::vec3 end = channel.positionKeys[nextPositionIndex].second;

        if (a_type == Interpolation::Type::Cubic)
        {
            // Get the indices of the control points
            uint v0i = positionIndex == 0 ? channel.positionKeys.size() - 1 : positionIndex - 1;
            uint v3i = nextPositionIndex == channel.positionKeys.size() - 1 ? 0 : nextPositionIndex + 1;

            // Get the control points
            glm::vec3 v0 = channel.positionKeys[v0i].second,
                      v3 = channel.positionKeys[v3i].second;
            
            return catmullRom(v0, start, end, v3, factor);
        }
        else return mix(start, end, factor);
    }

    glm::quat Model::calcInterpolatedRotation(float time, const BoneChannel& channel,
                                              Interpolation::Type a_type,
                                              Interpolation::EaseFn a_fn) const
    {
        if (channel.rotationKeys.empty())
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        if (channel.rotationKeys.size() == 1)
            return channel.rotationKeys[0].second;

        uint rotationIndex = [&]()
        {
            for (uint i = 0; i < channel.rotationKeys.size() - 1; i++)
                if (time < channel.rotationKeys[i + 1].first)
                    return i;
            return 0u;
        }();
        uint nextRotationIndex = rotationIndex + 1;
        float deltaTime = channel.rotationKeys[nextRotationIndex].first - channel.rotationKeys[rotationIndex].first;
        float factor = (time - channel.rotationKeys[rotationIndex].first) / deltaTime;

        if (a_fn) factor = a_fn(factor);
        
        glm::quat v0 = channel.rotationKeys[rotationIndex].second;
        glm::quat v1 = channel.rotationKeys[nextRotationIndex].second;

        if (a_type == Interpolation::Type::Cubic)
        {
            // Get the indices of the control points
            uint vn1i = rotationIndex == 0 ? channel.rotationKeys.size() - 1 : rotationIndex - 1;
            uint v2i  = nextRotationIndex == channel.rotationKeys.size() - 1 ? 0 : nextRotationIndex + 1;

            // Get the control points
            glm::quat vn1 = channel.rotationKeys[vn1i].second,
                      v2  = channel.rotationKeys[v2i].second;

            //glm::quat mix1 = mix(start, end, factor);
            //glm::quat mix2 = mix(intermediate(v0, start, end), intermediate(start, end, v3), factor);
            //glm::quat ret  = mix(mix1, mix2, 2.0f * (1.0f - factor) * factor);
            
            glm::quat ret = squad(v0, v1, intermediate(vn1, v0, v1), intermediate(v0, v1, v2), factor);
            //glm::quat ret = catmullRom(vn1, v0, v1, v2, factor);
            
            //return normalize(squad(start, end, intermediate(v0, start, end), intermediate(start, end, v3), factor));
            return normalize(ret);
        } else return slerp(v0, v1, factor);
        
        //return slerp(start, end, factor);
    }

    glm::vec3 Model::calcInterpolatedScale(float time, const BoneChannel& channel,
                                           Interpolation::Type a_type,
                                           Interpolation::EaseFn a_fn) const
    {
        if (channel.scaleKeys.empty())
            return glm::vec3(1.0f);
        if (channel.scaleKeys.size() == 1)
            return channel.scaleKeys[0].second;

        uint scaleIndex = [&]()
        {
            for (uint i = 0; i < channel.scaleKeys.size() - 1; i++)
                if (time < channel.scaleKeys[i + 1].first)
                    return i;
            return 0u;
        }();
        uint nextScaleIndex = scaleIndex + 1;
        float deltaTime = channel.scaleKeys[nextScaleIndex].first - channel.scaleKeys[scaleIndex].first;
        float factor = (time - channel.scaleKeys[scaleIndex].first) / deltaTime;

        if (a_fn) factor = a_fn(factor);
        
        glm::vec3 start = channel.scaleKeys[scaleIndex].second;
        glm::vec3 end = channel.scaleKeys[nextScaleIndex].second;

        if (a_type == Interpolation::Type::Cubic)
        {
            // Get the indices of the control points
            uint v0i = scaleIndex == 0 ? channel.scaleKeys.size() - 1 : scaleIndex - 1;
            uint v3i = nextScaleIndex == channel.scaleKeys.size() - 1 ? 0 : nextScaleIndex + 1;

            // Get the control points
            glm::vec3 v0 = channel.scaleKeys[v0i].second,
                      v3 = channel.scaleKeys[v3i].second;

            return catmullRom(v0, start, end, v3, factor);
        } else return mix(start, end, factor);
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
