#pragma once

#include "RendererAPI.h"

namespace oyl
{
    class RenderCommand
    {
    public:
        inline static void setClearColor(const glm::vec4& color)
        {
            s_rendererAPI->setClearColor(color.r, color.g, color.b, color.a);
        }

        inline static void setClearColor(float r, float g, float b, float a)
        {
            s_rendererAPI->setClearColor(r, g, b, a);
        }

        inline static void clear()
        {
            s_rendererAPI->clear();
        }

        inline static void setDepthDraw(bool value)
        {
            s_rendererAPI->setDepthDraw(value);
        }

        inline static void setBackfaceCulling(bool value)
        {
            s_rendererAPI->setBackfaceCulling(value);
        }

        inline static void drawArrays(const Ref<VertexArray>& vao, u32 count)
        {
            s_rendererAPI->drawArrays(vao, count);
        }

        inline static void drawIndexed(const Ref<VertexArray>& vao)
        {
            s_rendererAPI->drawIndexed(vao);
        }

        // TEMPORARY: Make more robust
        inline static void drawMesh(const Ref<Mesh>& mesh)
        {
            s_rendererAPI->drawMesh(mesh);
        }

    private:
        static RendererAPI* s_rendererAPI;
    };
}
