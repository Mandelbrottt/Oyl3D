#include "ScrollingTextureLayer.h"

void ScrollingTextureLayer::onEnter()
{

}

void ScrollingTextureLayer::onExit()
{
}

void ScrollingTextureLayer::onUpdate()
{
	auto view = registry->view<EntityInfo, Transform, Renderable>();
	view.each([&](EntityInfo& info, Transform& transform, Renderable& renderable)
		{
			if (info.name == "Water")
			{
				renderable.material->mainTextureProps.offset.y += 0.05f * Time::deltaTime();
				renderable.material->mainTextureProps.offset.x += -0.1f * Time::deltaTime();
			}
		});
}

void ScrollingTextureLayer::onGuiRender()
{
}

bool ScrollingTextureLayer::onEvent(const oyl::Event& event)
{
	return false;
}
