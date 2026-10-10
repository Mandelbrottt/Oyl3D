#include "MainLoop.h"

#include <Core/Application/Application.h>
#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>

#include <Graphics/Graphics.h>

namespace Oyl
{
	EditorClientMainLoop::EditorClientMainLoop()
	{
		auto graphicsModule = ModuleManager::LoadModulePointer<IGraphicsModule>("Graphics");

		m_mainWindow = graphicsModule->CreateWindow({
			.title = "Oyl3D - Now with more DLLs!",
			.size = Vector2u(1280, 720),
			.postEventCallback = PostEventDelegate::Create(this, &EditorClientMainLoop::PostEvent)
		});
	}

	EditorClientMainLoop::~EditorClientMainLoop()
	{
		m_mainWindow->Destroy();
		delete m_mainWindow;
		m_mainWindow = nullptr;

		ModuleManager::Get().UnloadModule("Graphics");
	}

	void
	EditorClientMainLoop::Update()
	{
		if (m_mainWindow && m_mainWindow->IsValid())
			m_mainWindow->Update();
	}

	void
	EditorClientMainLoop::PostEvent(const Event& a_event) const
	{
		if (a_event.eventType == typeidof(WindowCloseRequestEvent))
			Application::RequestExit(0);
	}
}
