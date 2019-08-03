#pragma once

#include "Graphics/Shader.h"

namespace oyl {

class OpenGLShader : public Shader {
public:
	virtual ~OpenGLShader();

	virtual void bind() const;
	virtual void unbind() const;

	virtual void setUniform1i(const std::string& name, const int v) override;
	virtual void setUniform2f(const std::string& name, const glm::vec2& v) override;
	virtual void setUniform3f(const std::string& name, const glm::vec3& v) override;
	virtual void setUniform4f(const std::string& name, const glm::vec4& v) override;
	virtual void setUniformMat3(const std::string& name, const glm::mat3& v) override;
	virtual void setUniformMat4(const std::string& name, const glm::mat4& v) override;
private:
	friend class Shader;
	OpenGLShader(const std::initializer_list<ShaderInfo>& files);

	void processShaders(const std::string& vertSrc,
						const std::string& tescSrc,
						const std::string& teseSrc,
						const std::string& geomSrc,
						const std::string& fragSrc);

	int getUniformLocation(const std::string& name);
private:
	uint m_rendererID = 0;

	std::unordered_map<std::string, int> m_uniformLocations;
};

}