#include "MainLoop.h"

#include <Core/Application/Application.h>
#include <Core/Modules/ModuleManager.h>

#include <Graphics/Graphics.h>

namespace Oyl
{
	EditorClientMainLoop::EditorClientMainLoop()
	{
		auto graphicsModule = ModuleManager::GetModulePointer<GraphicsModule>("Graphics");

		m_mainWindow = graphicsModule->CreateWindow({});
	}

	EditorClientMainLoop::~EditorClientMainLoop() {}

	void
	EditorClientMainLoop::Update() {}
}
