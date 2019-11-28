#pragma once

#include "Oyl3D/Rendering/RendererAPI.h"

namespace oyl
{
    class OpenGLRendererAPI : public RendererAPI
    {
        virtual void setClearColor(float r, float g, float b, float a) override;
        virtual void clear() override;

        virtual void setDepthDraw(bool value) override;
        virtual void setBackfaceCulling(bool value) override;

        virtual void drawArrays(const Ref<VertexArray>& vao, u32 count) override;
        virtual void drawIndexed(const Ref<VertexArray>& vao) override;
        virtual void drawMesh(const Ref<Mesh>& mesh) override;
    };
}
