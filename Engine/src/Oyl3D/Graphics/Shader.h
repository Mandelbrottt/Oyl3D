#pragma once

namespace oyl {

enum class ShaderType {
	Vertex,
	TessControl,
	TessEvaluation,
	Geometry,
	Pixel,
	NumTypes
};

struct ShaderInfo {
	ShaderType type;
	std::string filename;
};

class Shader {
public:
	virtual ~Shader() {}

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	static Shader* create(const std::initializer_list<ShaderInfo>& files);

	virtual void setUniformVec2(const std::string& name, const glm::vec2& v) = 0;
	virtual void setUniformVec3(const std::string& name, const glm::vec3& v) = 0;
	virtual void setUniformVec4(const std::string& name, const glm::vec4& v) = 0;
	virtual void setUniformMat3(const std::string& name, const glm::mat3& v) = 0;
	virtual void setUniformMat4(const std::string& name, const glm::mat4& v) = 0;
};

}