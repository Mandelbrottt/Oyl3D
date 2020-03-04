#pragma once

namespace oyl
{
    class VertexArray;
    class Mesh;
    
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
        virtual void setAlphaBlend(bool value) = 0;

        virtual void setDrawRect(int x, int y, int width, int height) = 0;

        virtual void drawArrays(const Ref<VertexArray>& vao, u32 count) = 0;
        virtual void drawIndexed(const Ref<VertexArray>& vao) = 0;

        static API getAPI() { return s_API; }

    protected:
        explicit RendererAPI() = default;
    private:
        static API s_API;
    };
}
