#include "oylpch.h"
#include "Mesh.h"

#include "Rendering/Renderer.h"

namespace oyl {

Mesh::Mesh(const std::string& filename) {
	loadFromFile(filename);
}

// Currently only supports Wavefront obj with v/vt/vn 3/2/3
bool Mesh::loadFromFile(const std::string& filename) {
	if (m_numFaces || m_numVertices)
		unload();

	std::ifstream file(filename);
	if (!file) {
		LOG_ERROR("Could not open file \"{0}\"!", filename);
		return false;
	}

	const int bufferSize = 128;
	char input[bufferSize];

	std::vector<glm::vec3> vertexData;
	std::vector<glm::vec2> textureData;
	std::vector<glm::vec3> normalData;

	std::vector<MeshFaceData> faceData;

	while (!file.eof()) {
		file.getline(input, bufferSize);

		if (std::strstr(input, "#") != nullptr) {
			// Comment
			continue;
		} else if (std::strstr(input, "vt") != nullptr) {
			// Vertex Texture UV Data
			glm::vec2 temp;
			sscanf(input, "vt %f %f", &temp.x, &temp.y);
			textureData.push_back(temp);
		} else if (std::strstr(input, "vn") != nullptr) {
			// Vertex Normal Data
			glm::vec3 temp;
			sscanf(input, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
			normalData.push_back(temp);
		} else if (std::strstr(input, "v") != nullptr) {
			// Vertex Position Data
			glm::vec3 temp;
			sscanf(input, "v %f %f %f", &temp.x, &temp.y, &temp.z);
			vertexData.push_back(temp);
		} else if (std::strstr(input, "f") != nullptr) {
			// Face Data
			uint tempVertices[3];
			uint tempTextureUVs[3];
			uint tempNormals[3];
			sscanf(input, "f %u/%u/%u %u/%u/%u %u/%u/%u",
				   tempVertices, tempTextureUVs, tempNormals,
				   tempVertices + 1, tempTextureUVs + 1, tempNormals + 1,
				   tempVertices + 2, tempTextureUVs + 2, tempNormals + 2);

			faceData.emplace_back(tempVertices[0], tempVertices[1], tempVertices[2],
								  tempTextureUVs[0], tempTextureUVs[1], tempTextureUVs[2],
								  tempNormals[0], tempNormals[1], tempNormals[2]);
		}
	}
	file.close();

	std::vector<float> unpackedData;
	unpackedData.reserve(faceData.size() * 8 * 3);

	for (auto& face : faceData) {
		for (int i = 0; i < 3; i++) {
			unpackedData.push_back(vertexData[face.vertices[i] - 1].x);
			unpackedData.push_back(vertexData[face.vertices[i] - 1].y);
			unpackedData.push_back(vertexData[face.vertices[i] - 1].z);

			unpackedData.push_back(textureData[face.textureUVs[i] - 1].x);
			unpackedData.push_back(textureData[face.textureUVs[i] - 1].y);

			unpackedData.push_back(normalData[face.normals[i] - 1].x);
			unpackedData.push_back(normalData[face.normals[i] - 1].y);
			unpackedData.push_back(normalData[face.normals[i] - 1].z);
		}
	}

	m_numFaces = faceData.size();
	m_numVertices = m_numFaces * 3;

	m_vbo.reset(VertexBuffer::create(unpackedData.data(), unpackedData.size() * sizeof(float)));

	BufferLayout layout = {
		{ Float3, "a_position" },
		{ Float2, "a_textureUV" },
		{ Float3, "a_normal" },
	};

	m_vbo->setLayout(layout);

	m_vao.reset(VertexArray::create());
	m_vao->addVertexBuffer(m_vbo);

	m_vbo->unbind();
	m_vao->unbind();

	return true;
}

void Mesh::unload() {
	m_vbo.reset();
	m_vao.reset();
}

void Mesh::loadTexture(const std::string& filename, uint slot) {
	m_texture.reset(Texture2D::create(filename));
}

void Mesh::loadTexture(std::shared_ptr<Texture2D> texture) {
	m_texture = texture;
}

void Mesh::unloadTexture() {
	m_texture.reset();
}

void Mesh::bind() {
	m_vao->bind();
	if (m_texture) m_texture->bind();
}

void Mesh::unbind() {
	m_vao->unbind();
	if (m_texture) m_texture->unbind();
}

}



