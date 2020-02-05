#include "CGLayer.h"

using namespace oyl;
using namespace component;

void CGLayer::onEnter()
{
    
}

void CGLayer::onExit()
{
    
}

void CGLayer::onUpdate()
{
    auto view = registry->view<EntityInfo, Transform, Renderable>();
    view.each([&](EntityInfo& info, Transform& transform, Renderable& renderable)
    {
        if (info.name == "Taurus 1")
        {
            glm::quat quat;
            quat.w = glm::cos(0.5f * Time::time());
            quat.x = glm::sin(2.0f * Time::time());
            quat.y = glm::cos(5.0f * Time::time());
            quat.z = glm::sin(3.0f * Time::time());
            transform.setRotation(normalize(quat));

            renderable.material->mainTextureProps.offset.y += 0.5f * Time::deltaTime();
        }
        else if (info.name == "Taurus 2")
        {
            glm::vec3 pos;
            pos.x = 0.5f * glm::sin(glm::pi<float>() * Time::time());
            pos.y = glm::cos(glm::pi<float>() * Time::time());
            pos.z = 0.5f * glm::cos(glm::pi<float>() * Time::time());
            transform.setPosition(5.0f * pos);
        }
    });
}

void CGLayer::onGuiRender()
{
    
}

bool CGLayer::onEvent(const oyl::Event& event)
{
    return false;
}
