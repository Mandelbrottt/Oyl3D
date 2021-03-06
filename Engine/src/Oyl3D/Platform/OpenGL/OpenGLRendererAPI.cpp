#include "oylpch.h"
#include "OpenGLRendererAPI.h"

#include "Graphics/Buffer.h"

#include <glad/glad.h>

namespace oyl
{
    void OpenGLRendererAPI::setClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void OpenGLRendererAPI::clear()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::setDepthDraw(bool value)
    {
        if (value)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        
        glDepthMask(value ? GL_TRUE : GL_FALSE);
    }

    void OpenGLRendererAPI::setBackfaceCulling(bool value)
    {
        if (value)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }

    void OpenGLRendererAPI::setDrawRect(int x, int y, int width, int height)
    {
        // TODO: Add clamping based on window size
        glViewport(x, y, width, height);
    }

    void OpenGLRendererAPI::drawArrays(const Ref<VertexArray>& vao, u32 count)
    {
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, count);
    }

    void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vao)
    {
        vao->bind();
        glDrawElements(GL_TRIANGLES, vao->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
    }

    //void OpenGLRendererAPI::drawMesh(const Ref<Mesh>& mesh)
    //{
    //    glDrawArrays(GL_TRIANGLES, 0, mesh->getNumVertices());
    //}
}
