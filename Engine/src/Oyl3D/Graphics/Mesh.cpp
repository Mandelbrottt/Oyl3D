#include "oylpch.h"
#include "Mesh.h"

#include "Rendering/Renderer.h"

#include "Utils/AssetCache.h"

namespace oyl
{
    //Ref<Mesh> Mesh::s_invalid;

    //std::unordered_map<std::string, Ref<Mesh>> Mesh::s_cache;

    internal::AssetCache<Mesh> Mesh::s_cache;

    const char* internal::AssetCache<Mesh>::s_typename = "Mesh";
    
    Mesh::Mesh(_Mesh, const std::string& filePath)
    {
        if (loadFromFile(filePath))
            m_filePath = filePath;
    }

    // Currently only supports Wavefront obj with v/vt/vn 3/2/3
    bool Mesh::loadFromFile(const std::string& filePath)
    {
        if (m_numFaces || m_numVertices)
            unload();

        std::ifstream file(filePath);
        if (!file)
        {
            OYL_LOG_ERROR("Could not open file \"{0}\"!", filePath);
            return false;
        }

        const int bufferSize = 512;
        char      input[bufferSize];

        std::vector<glm::vec3> positionData;
        std::vector<glm::vec2> textureData;
        std::vector<glm::vec3> normalData;

        using Face = std::array<std::array<int, 3>, 3>;

        std::vector<Face> faceData;

        while (!file.eof())
        {
            file.getline(input, bufferSize);

            switch (input[0])
            {
                case '#':
                    break;
                case 'v':
                    if (input[1] == ' ')
                    {
                        glm::vec3 temp;

                        int result = sscanf_s(input,
                                              "v %f %f %f",
                                              &temp[0],
                                              &temp[1],
                                              &temp[2]);
                        OYL_ASSERT(result == 3, "Not all values read in!");
                        positionData.emplace_back(temp);
                    }
                    else if (input[1] == 't')
                    {
                        glm::vec2 temp;

                        int result = sscanf_s(input,
                                              "vt %f %f",
                                              &temp[0],
                                              &temp[1]);
                        OYL_ASSERT(result == 2, "Not all values read in!");
                        textureData.emplace_back(temp);
                    }
                    else if (input[1] == 'n')
                    {
                        glm::vec3 temp;

                        int result = sscanf_s(input,
                                              "vn %f %f %f",
                                              &temp[0],
                                              &temp[1],
                                              &temp[2]);
                        OYL_ASSERT(result == 3, "Not all values read in!");
                        normalData.emplace_back(temp);
                    }
                    break;
                case 'f':
                    Face temp;

                    int result = sscanf_s(input,
                                          "f %d/%d/%d %d/%d/%d %d/%d/%d",
                                          &temp[0][0], &temp[0][1], &temp[0][2],
                                          &temp[1][0], &temp[1][1], &temp[1][2],
                                          &temp[2][0], &temp[2][1], &temp[2][2]);
                    OYL_ASSERT(result == 9, "Not all values read in!");
                    faceData.emplace_back(temp);
                    break;
            }
        }
        file.close();

        std::vector<float> unpackedData;
        unpackedData.reserve(faceData.size() * 8 * 3);

        for (auto& face : faceData)
        {
            for (auto& attribute : face)
            {
                unpackedData.push_back(positionData[attribute[0] - 1].x);
                unpackedData.push_back(positionData[attribute[0] - 1].y);
                unpackedData.push_back(positionData[attribute[0] - 1].z);

                unpackedData.push_back(textureData[attribute[1] - 1].x);
                unpackedData.push_back(textureData[attribute[1] - 1].y);

                unpackedData.push_back(normalData[attribute[2] - 1].x);
                unpackedData.push_back(normalData[attribute[2] - 1].y);
                unpackedData.push_back(normalData[attribute[2] - 1].z);
            }
        }

        m_numFaces    = faceData.size();
        m_numVertices = m_numFaces * 3;

        m_vbo = VertexBuffer::create(unpackedData.data(), unpackedData.size() * sizeof(float));

        BufferLayout layout = {
            { DataType::Float3, "a_position" },
            { DataType::Float2, "a_textureUV" },
            { DataType::Float3, "a_normal" },
        };

        m_vbo->setLayout(layout);

        m_vao = VertexArray::create();
        m_vao->addVertexBuffer(m_vbo);

        m_vbo->unbind();
        m_vao->unbind();

        return true;
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

    Ref<Mesh> Mesh::create(const std::string& filePath)
    {
        auto mesh = Ref<Mesh>::create(_Mesh{}, filePath);
        return mesh->m_numVertices <= 0 ? s_cache.get(INVALID_ALIAS) : mesh;
    }

    const Ref<Mesh>& Mesh::cache(const std::string& filePath,
                                 const CacheAlias&  alias,
                                 bool overwrite)
    {
        return s_cache.cache(filePath, alias, overwrite);
    }

    const Ref<Mesh>& Mesh::cache(const Ref<Mesh>&  existing,
                                 const CacheAlias& alias,
                                 bool overwrite)
    {
        return s_cache.cache(existing, alias, overwrite);
    }

    void Mesh::discard(const CacheAlias& alias)
    {
        s_cache.discard(alias);
    }

    const Ref<Mesh>& Mesh::get(const CacheAlias& alias)
    {
        return s_cache.get(alias);
    }

    bool Mesh::isCached(const Ref<Mesh>& existing)
    {
        return s_cache.isCached(existing);
    }

    bool Mesh::exists(const CacheAlias& alias)
    {
        return s_cache.exists(alias);
    }

    const CacheAlias& Mesh::getAlias(const Ref<Mesh>& existing)
    {
        return s_cache.getAlias(existing);
    }

    const Ref<Mesh>& Mesh::rename(const CacheAlias& currentAlias,
                                  const CacheAlias& newAlias,
                                  bool              overwrite)
    {
        return s_cache.rename(currentAlias, newAlias, overwrite);
    }

    void Mesh::init()
    {
        Mesh::cache(ENGINE_RES + INVALID_MESH_PATH, INVALID_ALIAS);
    }
}
