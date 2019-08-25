#pragma once

namespace oyl {

class Texture1D {
public:
	virtual ~Texture1D() {}

	virtual void load(const std::string& filename) = 0;
	virtual void unload() = 0;

	virtual void bind() = 0;
	virtual void unbind() = 0;

	static Ref<Texture1D> create(const std::string& filename);
};

class Texture2D {
public:
	virtual ~Texture2D() {}

	virtual void load(const std::string& filename) = 0;
	virtual void unload() = 0;

	virtual void bind() = 0;
	virtual void unbind() = 0;

	static Ref<Texture2D> create(const std::string& filename);
};

class Texture3D {
public:
	virtual ~Texture3D() {}

	virtual void load(const std::string& filename) = 0;
	virtual void unload() = 0;

	virtual void bind() = 0;
	virtual void unbind() = 0;

	static Ref<Texture3D> create(const std::string& filename);
};

}
