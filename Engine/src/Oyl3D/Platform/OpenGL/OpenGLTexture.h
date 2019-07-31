#pragma once

#include "Graphics/Texture.h"

namespace oyl {

class OpenGLTexture1D : public Texture1D {
public:
	OpenGLTexture1D(const std::string& filename);
	virtual ~OpenGLTexture1D();

	virtual void load(const std::string& filename) override;
	virtual void unload() override;

	virtual void bind() override;
	virtual void unbind() override;
private:
	uint m_rendererID = 0;
};

class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(const std::string& filename);
	virtual ~OpenGLTexture2D();

	virtual void load(const std::string& filename) override;
	virtual void unload() override;

	virtual void bind() override;
	virtual void unbind() override;
private:
	uint m_rendererID = 0;
};

class OpenGLTexture3D : public Texture3D {
public:
	OpenGLTexture3D(const std::string& filename);
	virtual ~OpenGLTexture3D();

	virtual void load(const std::string& filename) override;
	virtual void unload() override;

	virtual void bind() override;
	virtual void unbind() override;
private:
	uint m_rendererID = 0;
};

}