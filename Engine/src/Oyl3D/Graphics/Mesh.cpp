#include "oylpch.h"
#include "Mesh.h"

#include "Graphics/Buffer.h"

namespace oyl
{    
    Mesh::Mesh(VertexData* a_vertices, uint a_numVertices)
    {
        loadFromVertices(a_vertices, a_numVertices);
    }

    void Mesh::loadFromVertices(VertexData* a_vertices, uint a_numVertices)
    {
        if (!a_vertices || a_numVertices == 0)
            return;
        
        m_vbo = VertexBuffer::create(reinterpret_cast<float*>(a_vertices), a_numVertices * sizeof(VertexData));

        BufferLayout layout = {
            { DataType::Float3, "in_position" },
            { DataType::Float2, "in_textureUV" },
            { DataType::Float3, "in_normal" },
            { DataType::Float3, "in_tangent" },
            { DataType::Float3, "in_biTangent" },
        };

        m_vbo->setLayout(layout);

        m_vao = VertexArray::create();
        m_vao->addVertexBuffer(m_vbo);

        m_vbo->unbind();
        m_vao->unbind();
        
        m_numVertices = a_numVertices;
        m_numFaces = a_numVertices / 3;
    }

    void Mesh::unload()
    {
        m_vbo.reset();
        m_vao.reset();
    }

    void Mesh::bind()
    {
        m_vao->bind();
    }

    void Mesh::unbind()
    {
        m_vao->unbind();
    }
}
