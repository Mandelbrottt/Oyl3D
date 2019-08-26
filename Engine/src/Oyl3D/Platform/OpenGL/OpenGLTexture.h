#pragma once

#include "Graphics/Texture.h"

namespace oyl {

class OpenGLTexture1D : public Texture1D {
public:
	OpenGLTexture1D(const std::string& filename);
	virtual ~OpenGLTexture1D();

	virtual void load(const std::string& filename) override;
	virtual void unload() override;

	virtual void bind(uint slot) const override;
	virtual void unbind() const override;

	virtual uint getLength() const override { return m_length; }
private:
	uint m_rendererID = 0;

	uint m_length = 0;

	std::string m_path;

	bool m_loaded = false;
};

class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(const std::string& filename);
	virtual ~OpenGLTexture2D();

	virtual void load(const std::string& filename) override;
	virtual void unload() override;

	virtual void bind(uint slot) const override;
	virtual void unbind() const override;

	virtual uint getWidth() const override { return m_width; }
	virtual uint getHeight() const override { return m_height; }
private:
	uint m_rendererID = 0;

	uint m_width = 0;
	uint m_height = 0;

	std::string m_path;

	bool m_loaded = false;
};

class OpenGLTexture3D : public Texture3D {
public:
	OpenGLTexture3D(const std::string& filename);
	virtual ~OpenGLTexture3D();

	virtual void load(const std::string& filename) override;
	virtual void unload() override;

	virtual void bind(uint slot) const override;
	virtual void unbind() const override;

	virtual uint getWidth() const override { return m_width; }
	virtual uint getHeight() const override { return m_height; }
	virtual uint getDepth() const override { return m_depth; }
private:
	uint m_rendererID = 0;

	uint m_width = 0;
	uint m_height = 0;
	uint m_depth = 0;

	std::string m_path;

	bool m_loaded = false;
};

}