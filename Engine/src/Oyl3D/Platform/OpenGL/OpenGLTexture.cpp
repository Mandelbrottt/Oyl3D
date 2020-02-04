#include "oylpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace oyl
{   
    static GLenum FilterToGL(TextureFilter a_filter)
    {
        GLenum filter = GL_NONE;
        switch (a_filter)
        {
            case TextureFilter::Nearest:
                filter = GL_NEAREST;
                break;
            case TextureFilter::Linear:
                filter = GL_LINEAR;
                break;
        }
        return filter;
    }

    static GLenum WrapToGL(TextureWrap a_wrap)
    {
        GLenum wrap = GL_NONE;
        switch (a_wrap)
        {
            case TextureWrap::Repeat:
                wrap = GL_REPEAT;
                break;
            case TextureWrap::Mirror:
                wrap = GL_MIRRORED_REPEAT;
                break;
            case TextureWrap::ClampToEdge:
                wrap = GL_CLAMP_TO_EDGE;
                break;
            case TextureWrap::ClampToBorder:
                wrap = GL_CLAMP_TO_BORDER;
                break;
        }
        return wrap;
    }

    OpenGLTexture1D::OpenGLTexture1D(const std::string& filename,
                                     TextureFilter  a_filter,
                                     TextureWrap    a_wrap,
                                     TextureProfile a_profile)
    {
        if (load(filename, a_filter, a_wrap, a_profile))
            m_filePath = filename;
    }

    OpenGLTexture1D::~OpenGLTexture1D()
    {
        unload();
    }

    bool OpenGLTexture1D::load(const std::string& filename,
                               TextureFilter  a_filter,
                               TextureWrap    a_wrap,
                               TextureProfile a_profile)
    {
        return false;
    }

    void OpenGLTexture1D::unload()
    {
    }

    void OpenGLTexture1D::bind(uint slot) const
    {
    }

    void OpenGLTexture1D::unbind(uint slot) const
    {
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string& filename,
                                     TextureFilter  a_filter,
                                     TextureWrap    a_wrap,
                                     TextureProfile a_profile)
    {
        if (load(filename, a_filter, a_wrap, a_profile))
        {
            m_filePath = filename;
            m_filter = a_filter;
            m_wrap = a_wrap;
            m_profile = a_profile;
        }
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        unload();
    }

    bool OpenGLTexture2D::load(const std::string& filename,
                               TextureFilter  a_filter,
                               TextureWrap    a_wrap,
                               TextureProfile a_profile)
    {
        if (m_loaded) unload();

        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_2D, m_rendererID);

        GLenum filter = FilterToGL(a_filter);
        GLenum wrap = WrapToGL(a_wrap);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        u8* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            m_loaded = true;
            m_width  = width;
            m_height = height;

            GLenum texStorageFormat = GL_NONE;
            GLenum texSubFormat     = GL_NONE;

            switch (nrChannels)
            {
                case 1:
                    texStorageFormat = GL_R8;
                    texSubFormat     = GL_RED;
                    break;
                case 2:
                    texStorageFormat = GL_RG8;
                    texSubFormat     = GL_RG;
                    break;
                case 3:
                    texStorageFormat = a_profile == TextureProfile::SRGB ? GL_SRGB8 : GL_RGB8;
                    texSubFormat     = GL_RGB;
                    break;
                case 4:
                    texStorageFormat = a_profile == TextureProfile::SRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
                    texSubFormat     = GL_RGBA;
                    break;
            }

            if (texStorageFormat != GL_NONE && texSubFormat != GL_NONE)
            {
                glTexStorage2D(GL_TEXTURE_2D, 1, texStorageFormat, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, texSubFormat, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
        }
        else
        {
            OYL_LOG_ERROR("Texture '{0}' failed to load!", filename);
            
            u8 err[12] = 
            {
                0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
                0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF
            };

            filter = GL_NEAREST;
            wrap = GL_REPEAT;

            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 2, 2);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, 2, GL_RGB, GL_UNSIGNED_BYTE, err);
        }
        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

        return m_loaded;
    }

    void OpenGLTexture2D::unload()
    {
        if (!m_loaded) return;
        m_loaded = false;

        glDeleteTextures(1, &m_rendererID);
    }

    void OpenGLTexture2D::bind(uint slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_rendererID);
    }

    void OpenGLTexture2D::unbind(uint slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    void OpenGLTexture2D::setFilter(TextureFilter a_filter)
    {
        glBindTexture(GL_TEXTURE_2D, m_rendererID);
        
        GLenum filter = FilterToGL(a_filter);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        
        m_filter = a_filter;
    }
    
    void OpenGLTexture2D::setWrap(TextureWrap a_wrap)
    {
        glBindTexture(GL_TEXTURE_2D, m_rendererID);

        GLenum wrap = WrapToGL(a_wrap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

        m_wrap = a_wrap;
    }

    void OpenGLTexture2D::setProfile(TextureProfile a_profile)
    {
        if (m_loaded && load(m_filePath, m_filter, m_wrap, a_profile))
            m_profile = a_profile;
    }

    OpenGLTexture3D::OpenGLTexture3D(const std::string& filename,
                                     TextureFilter  a_filter,
                                     TextureWrap    a_wrap,
                                     TextureProfile a_profile)
    {
        if (load(filename, a_filter, a_wrap, a_profile))
            m_filePath = filename;
    }

    OpenGLTexture3D::~OpenGLTexture3D()
    {
        unload();
    }

    bool OpenGLTexture3D::load(const std::string& filename,
                               TextureFilter  a_filter,
                               TextureWrap    a_wrap,
                               TextureProfile a_profile)
    {
        if (m_loaded) unload();

        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_3D, m_rendererID);

        GLenum filter = FilterToGL(a_filter);
        GLenum wrap = WrapToGL(a_wrap);

        std::vector<u8> lut;

        int size = 0;
        bool is1D = false;
        glm::vec3 lowDomain(0.0f), highDomain(1.0f);
        
        std::string line;
        line.reserve(256);
        std::string keyword;
        keyword.reserve(256);
        
        std::ifstream file(filename, std::ios::binary);
        if (!file) return false;
        
        while (!file.eof())
        {
            size_t linePos = file.tellg();
            std::getline(file, line);

            if (line[0] == '#')
                continue;

            std::istringstream stream(line);
            stream >> keyword;

            if ("+" < keyword && keyword < ":")
            {
                file.seekg(linePos);
                break;
            }
            else if (keyword == "LUT_1D_SIZE")
            {
                stream >> size;
                if (size < 2 || size > 65536)
                    return false;
                lut.resize(size);
                is1D = true;
            }
            else if (keyword == "LUT_3D_SIZE")
            {
                stream >> size;
                if (size < 2 || size > 256)
                    return false;
                lut.resize(size * size * size * 3);
            }
            else if (keyword == "DOMAIN_MIN")
            {
                stream >> lowDomain.x >> lowDomain.y >> lowDomain.z;
            }
            else if (keyword == "DOMAIN_MAX")
            {
                stream >> highDomain.x >> highDomain.y >> highDomain.z;
            }
        }

        if (is1D)
        {
            for (int i =  0; i < size && !file.eof(); i++)
            {
                glm::vec3 input;
                glm::vec<3, u8> store{};
                std::getline(file, line);
                if (file.fail()) return false;

                std::istringstream stream(line);
                stream >> input.r >> input.g >> input.b;
                input -= lowDomain;
                input /= highDomain - lowDomain;
                store = input;
                lut[i] = store.r, lut[i + 1] = store.g, lut[i + 2] = store.b;
            }
        }
        else
        {
            for (int b = 0; b < size; b++)
                for (int g = 0; g < size; g++)
                    for (int r = 0; r < size; r++)
                    {
                        glm::vec3 input;
                        glm::vec<3, u8> store{};
                        std::getline(file, line);
                        if (file.fail()) return false;

                        std::istringstream stream(line);
                        stream >> input.r >> input.g >> input.b;
                        input -= lowDomain;
                        input /= highDomain - lowDomain;
                        store = input * 255.0f;
                        int index = 3 * (r + size * g + size * size * b);
                        lut[index] = store.r, lut[index + 1] = store.g, lut[index + 2] = store.b;
                    }
        }

        m_loaded = true;
        
        GLenum texStorageFormat = GL_RGB8;
        GLenum texSubFormat = GL_RGB;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (texStorageFormat != GL_NONE && texSubFormat != GL_NONE)
        {
            glTexStorage3D(GL_TEXTURE_3D, 1, texStorageFormat, size, size, size);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, size, size, size, texSubFormat, GL_UNSIGNED_BYTE, lut.data());
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);

        return m_loaded;
    }

    void OpenGLTexture3D::unload()
    {
        if (!m_loaded) return;
        m_loaded = false;

        glDeleteTextures(1, &m_rendererID);
    }

    void OpenGLTexture3D::bind(uint slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_3D, m_rendererID);
    }

    void OpenGLTexture3D::unbind(uint slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_3D, GL_NONE);
    }

    OpenGLTextureCubeMap::OpenGLTextureCubeMap(const std::string& filename,
                                               TextureFilter  a_filter,
                                               TextureWrap    a_wrap,
                                               TextureProfile a_profile)
    {
        if (load(filename, a_filter, a_wrap, a_profile))
            m_filePath = filename;
    }
    
    OpenGLTextureCubeMap::~OpenGLTextureCubeMap()
    {
        unload();
    }

    bool OpenGLTextureCubeMap::load(const std::string& filename, 
                                    TextureFilter  a_filter,
                                    TextureWrap    a_wrap,
                                    TextureProfile a_profile)
    {
        if (m_loaded) unload();

        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        const char* suffixes[6] = {
            "_rt", "_lf",
            "_up", "_dn",
            "_bk", "_ft",
        };

        for (int i = 0; i < 6; ++i)
        {
            std::string finalFilePath = filename;

            auto lastDot = finalFilePath.rfind('.');
            finalFilePath.insert(lastDot, suffixes[i]);
            
            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(false);
            u8* data = stbi_load(finalFilePath.c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                m_loaded = true;
                m_width = width;
                m_height = height;

                auto texStorageFormat = nrChannels == 3 ? GL_RGB8 : GL_RGBA8;
                auto texSubFormat     = nrChannels == 3 ? GL_RGB  : GL_RGBA;

                if (i == 0)
                    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, texStorageFormat, width, height);
                glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                                0, 0, 0, width, height, 
                                texSubFormat, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                OYL_LOG_ERROR("Texture '{0}' failed to load!", finalFilePath);

                u8 err[12] =
                {
                    0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
                    0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF
                };

                if (i == 0)
                    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB8, 2, 2);
                glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, 2, 2, GL_RGB, GL_UNSIGNED_BYTE, err);
            }
            stbi_image_free(data);
        }

        return m_loaded;
    }

    void OpenGLTextureCubeMap::unload()
    {
        if (!m_loaded) return;
        m_loaded = false;

        glDeleteTextures(1, &m_rendererID);
    }

    void OpenGLTextureCubeMap::bind(uint slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);
    }

    void OpenGLTextureCubeMap::unbind(uint slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
    }
}
