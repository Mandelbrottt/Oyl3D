#pragma once

namespace oyl {

enum class ShaderType {
	None,
	Vertex,
	Geometry,
	Pixel
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

	static Shader* create(const std::string& vertexSrc, const std::string& fragmentSrc);
	static Shader* create(const std::initializer_list<ShaderInfo>& files);
};

}