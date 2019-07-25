#pragma once

#include "Oyl3D/Graphics/Buffer.h"

namespace oyl {

class Mesh {
public:
	virtual bool loadFromFile(const std::string& filename);
	virtual void unload();

	void bind();
	void unbind();

	static Mesh* create(const std::string& filename) { return new Mesh(filename); }

	uint getNumFaces() const { return m_numFaces; }
	uint getNumVertices() const { return m_numVertices; }
private:
	Mesh(const std::string& filename);

	uint m_numFaces = 0;
	uint m_numVertices = 0;

	std::shared_ptr<VertexArray> m_vao;
	std::shared_ptr<VertexBuffer> m_vbo;
};

struct MeshFaceData {
	MeshFaceData(uint v0, uint v1, uint v2,
				 uint t0, uint t1, uint t2,
				 uint n0, uint n1, uint n2) {
		init(v0, v1, v2, t0, t1, t2, n0, n1, n2);
	}

	void init(uint v0, uint v1, uint v2,
			  uint t0, uint t1, uint t2,
			  uint n0, uint n1, uint n2) {

		vertices[0] = v0;
		vertices[1] = v1;
		vertices[2] = v2;

		textureUVs[0] = t0;
		textureUVs[1] = t1;
		textureUVs[2] = t2;

		normals[0] = n0;
		normals[1] = n1;
		normals[2] = n2;
	}

	uint vertices[3];
	uint textureUVs[3];
	uint normals[3];
};

}