#pragma once

#include "Graphics/Texture.h"

namespace oyl
{
    class OpenGLTexture1D : public Texture1D
    {
    public:
        explicit OpenGLTexture1D(const std::string& filename);
        virtual ~OpenGLTexture1D();

        virtual bool load(const std::string& filename) override;
        virtual void unload() override;

        virtual void bind(uint slot) const override;
        virtual void unbind(uint slot) const override;

        virtual bool isLoaded() const override { return m_loaded; }

        virtual uint getLength() const override { return m_length; }
    protected:
        uint m_rendererID = 0;
        uint m_length     = 0;

        bool m_loaded = false;
    };

    class OpenGLTexture2D : public Texture2D
    {
    public:
        explicit OpenGLTexture2D(const std::string& filename);
        virtual ~OpenGLTexture2D();

        virtual bool load(const std::string& filename) override;
        virtual void unload() override;

        virtual void bind(uint slot) const override;
        virtual void unbind(uint slot) const override;

        virtual bool isLoaded() const override { return m_loaded; }

        virtual uint getWidth() const override { return m_width; }
        virtual uint getHeight() const override { return m_height; }

    protected:
        uint m_rendererID = 0;
        uint m_width      = 0;
        uint m_height     = 0;

        bool m_loaded = false;
    };

    class OpenGLTexture3D : public Texture3D
    {
    public:
        explicit OpenGLTexture3D(const std::string& filename);
        virtual ~OpenGLTexture3D();

        virtual bool load(const std::string& filename) override;
        virtual void unload() override;

        virtual void bind(uint slot) const override;
        virtual void unbind(uint slot) const override;

        virtual bool isLoaded() const override { return m_loaded; }

        virtual uint getWidth() const override { return m_width; }
        virtual uint getHeight() const override { return m_height; }
        virtual uint getDepth() const override { return m_depth; }

    protected:
        uint m_rendererID = 0;
        uint m_width      = 0;
        uint m_height     = 0;
        uint m_depth      = 0;

        bool m_loaded = false;
    };
}
