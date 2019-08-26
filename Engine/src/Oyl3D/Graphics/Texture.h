#pragma once

namespace oyl {

class Texture {
public:
	virtual ~Texture() {}

	virtual void load(const std::string& filename) = 0;
	virtual void unload() = 0;

	virtual void bind(uint slot = 0) const = 0;
	virtual void unbind() const = 0;
};

class Texture1D : public Texture {
public:
	virtual ~Texture1D() {}

	virtual uint getLength() const = 0;

	static Ref<Texture1D> create(const std::string& filename);
};

class Texture2D : public Texture {
public:
	virtual ~Texture2D() {}

	virtual uint getWidth() const = 0;
	virtual uint getHeight() const = 0;

	static Ref<Texture2D> create(const std::string& filename);
};

class Texture3D : public Texture {
public:
	virtual ~Texture3D() {}

	virtual uint getWidth() const = 0;
	virtual uint getHeight() const = 0;
	virtual uint getDepth() const = 0;

	static Ref<Texture3D> create(const std::string& filename);
};

}
