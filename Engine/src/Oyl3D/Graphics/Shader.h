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
};

}