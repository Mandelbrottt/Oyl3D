#include "oylpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace oyl
{
    OpenGLTexture1D::OpenGLTexture1D(const std::string& filename)
    {
        if (load(filename))
            m_filePath = filename;
    }

    OpenGLTexture1D::~OpenGLTexture1D()
    {
        unload();
    }

    bool OpenGLTexture1D::load(const std::string& filename)
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

    OpenGLTexture2D::OpenGLTexture2D(const std::string& filename)
    {
        if (load(filename))
            m_filePath = filename;
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        unload();
    }

    bool OpenGLTexture2D::load(const std::string& filename)
    {
        if (m_loaded) unload();

        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_2D, m_rendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
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
                    texStorageFormat = GL_SRGB8;
                    texSubFormat     = GL_RGB;
                    break;
                case 4:
                    texStorageFormat = GL_SRGB8_ALPHA8;
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

            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 2, 2);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, 2, GL_RGB, GL_UNSIGNED_BYTE, err);
        }
        stbi_image_free(data);

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

    OpenGLTexture3D::OpenGLTexture3D(const std::string& filename)
    {
        if (load(filename))
            m_filePath = filename;
    }

    OpenGLTexture3D::~OpenGLTexture3D()
    {
        unload();
    }

    bool OpenGLTexture3D::load(const std::string& filename)
    {
        return false;
    }

    void OpenGLTexture3D::unload() { }

    void OpenGLTexture3D::bind(uint slot) const { }

    void OpenGLTexture3D::unbind(uint slot) const { }

    OpenGLTextureCubeMap::OpenGLTextureCubeMap(const std::string& filename)
    {
        if (load(filename))
            m_filePath = filename;
    }
    
    OpenGLTextureCubeMap::~OpenGLTextureCubeMap()
    {
        unload();
    }

    bool OpenGLTextureCubeMap::load(const std::string& filename)
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
