#pragma once

#include "Oyl3D/Graphics/Buffer.h"

#include "Oyl3D/Graphics/Texture.h"

namespace oyl
{
    class Mesh
    {
    public:
        bool loadFromFile(const std::string& filename);
        void unload();

        void loadTexture(const std::string& filename, uint slot = 0);
        void loadTexture(Ref<Texture2D> texture);
        void unloadTexture();

        void bind();
        void unbind();

        static Ref<Mesh> create(const std::string& filename) { return Ref<Mesh>(new Mesh(filename)); }

        uint getNumFaces() const { return m_numFaces; }
        uint getNumVertices() const { return m_numVertices; }

        Ref<Texture2D> getTexture() { return Ref<Texture2D>(m_texture); }
    private:
        Mesh(const std::string& filename);

        uint m_numFaces    = 0;
        uint m_numVertices = 0;

        Ref<VertexArray>  m_vao;
        Ref<VertexBuffer> m_vbo;

        Ref<Texture2D> m_texture;
    };

    struct MeshFaceData
    {
        MeshFaceData(uint v0, uint v1, uint v2,
                     uint t0, uint t1, uint t2,
                     uint n0, uint n1, uint n2)
        {
            init(v0, v1, v2, t0, t1, t2, n0, n1, n2);
        }

        void init(uint v0, uint v1, uint v2,
                  uint t0, uint t1, uint t2,
                  uint n0, uint n1, uint n2)
        {
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
