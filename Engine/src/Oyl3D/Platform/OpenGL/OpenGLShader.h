#pragma once

#include "Graphics/Shader.h"

namespace oyl {

class OpenGLShader : public Shader {
public:
	OpenGLShader(const std::initializer_list<ShaderInfo>& files);
	virtual ~OpenGLShader();

	virtual void bind() const;
	virtual void unbind() const;
private:
	void processShaders(const std::string& vertSrc, 
						const std::string& tescSrc, 
						const std::string& teseSrc, 
						const std::string& geomSrc, 
						const std::string& fragSrc);
private:
	uint m_rendererID;
};

}