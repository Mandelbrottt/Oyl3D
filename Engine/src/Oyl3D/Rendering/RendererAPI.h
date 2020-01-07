#pragma once

#include "Oyl3D/Graphics/Buffer.h"
#include "Oyl3D/Graphics/Mesh.h"

namespace oyl
{
    class RendererAPI
    {
    public:
        enum API
        {
            OpenGL,
            DirectX,
            Metal,
        };
        
    public:
        virtual ~RendererAPI() = default;
        
        virtual void setClearColor(float r, float g, float b, float a) = 0;
        virtual void clear() = 0;

        virtual void setDepthDraw(bool value) = 0;
        virtual void setBackfaceCulling(bool value) = 0;

        virtual void setDrawRect(int x, int y, int width, int height) = 0;

        virtual void drawArrays(const Ref<VertexArray>& vao, u32 count) = 0;
        virtual void drawIndexed(const Ref<VertexArray>& vao) = 0;
        virtual void drawMesh(const Ref<Mesh>& mesh) = 0;

        static API getAPI() { return s_API; }

    protected:
        explicit RendererAPI() = default;
    private:
        static API s_API;
    };
}
