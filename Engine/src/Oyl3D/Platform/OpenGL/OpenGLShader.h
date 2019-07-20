#pragma once

#include "Graphics/Shader.h"

namespace oyl {

class OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
	virtual ~OpenGLShader();

	virtual void bind() const;
	virtual void unbind() const;
private:
	uint m_rendererID;
};

}