#pragma once

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Graphics/Material.h"
#include "Oyl3D/Graphics/Texture.h"

namespace oyl
{
    class Mesh
    {
        struct _Mesh
        {
        };

    public:
        explicit Mesh(_Mesh, const std::string& filename);

        bool loadFromFile(const std::string& filename);
        void unload();

        void bind();
        void unbind();

        static Ref<Mesh> create(const std::string& filename) { return Ref<Mesh>::create(_Mesh{}, filename); }

        uint getNumFaces() const { return m_numFaces; }
        uint getNumVertices() const { return m_numVertices; }

    protected:
        uint m_numFaces    = 0;
        uint m_numVertices = 0;

        Ref<VertexArray>  m_vao = nullptr;
        Ref<VertexBuffer> m_vbo = nullptr;
    };
}
