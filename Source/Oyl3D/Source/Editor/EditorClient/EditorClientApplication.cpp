#include "EditorClientApplication.h"

#include <Application/Application.h>

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Profiling/Profiling.h>
#include <Core/Time/Time.h>

#include <Rendering/Renderer.h>
#include <Rendering/Rendering.h>

namespace Oyl
{
	void
	EditorClientApplication::OnStart()
	{
		OYL_PROFILE_FUNCTION();

		ModuleLoadResult loadResult;
		auto applicationModule = ModuleManager::LoadModulePointer("Application", &loadResult);

		if (!applicationModule)
			LogError("Failed to load ApplicationModule with error \"{}\"!",
			         Traits::TUnderlyingType<decltype(loadResult)>(loadResult));

		LogTrace("Starting Main Window");
		CreateApplicationWindow({
			.title = "Oyl3D - Now with more DLLs!",
			.size = Vector2u(1280, 720)
		});

		if (!m_applicationWindows.empty())
			m_mainWindow = m_applicationWindows[0];

		if (m_mainWindow)
		{
			m_mainWindow->RegisterWindowCloseRequestCallback({ this, &EditorClientApplication::OnWindowCloseRequestEvent });
			m_mainWindow->RegisterKeyPressCallback({ this, &EditorClientApplication::OnKeyPressEvent });
		}

		auto* renderingModule = ModuleManager::GetModulePointer<IRenderingModule>("Rendering");
		m_renderer = renderingModule->CreateRenderer(RenderingApi::Vulkan);
		if (m_renderer)
		{
			m_renderer->Init(m_mainWindow);
			for (auto* window : m_applicationWindows)
				m_renderer->RegisterApplicationWindow(*window);
		}
	}

	void
	EditorClientApplication::OnStop()
	{
		OYL_PROFILE_FUNCTION();

		if (m_renderer)
		{
			for (auto* window : m_applicationWindows)
				m_renderer->UnregisterApplicationWindow(*window);
			m_renderer->Shutdown();
		}

		while (!m_applicationWindows.empty())
			CloseApplicationWindow(m_applicationWindows.front());

		m_applicationWindows.clear();
		m_mainWindow = nullptr;
	}

	void
	EditorClientApplication::OnUpdate()
	{
		OYL_PROFILE_FUNCTION();

		m_targetFrameRate = m_targetFramerateHint;

		if (m_mainWindow && m_mainWindow->IsValid())
			m_mainWindow->Update();

		if (m_renderer)
			m_renderer->Update();
	}

	void
	EditorClientApplication::WaitIdle()
	{
		OYL_PROFILE_FUNCTION();

		auto& frameTimer = Time::Internal::GetFrameTimer();

		// Intentionally use g_frameTimer instead of Oyl::Time::GetFrameTimer since that is user overrideable
		static auto lastTime = frameTimer.ElapsedProcessorTime();

		// If target framerate is 0, don't cap framerate
		uint32 targetFrameRate = TargetFramerate();
		float64 minTimePerFrame = 0;
		if (targetFrameRate > 0)
			minTimePerFrame = 1.0 / targetFrameRate;

		auto targetTimeForNextFrame = lastTime + minTimePerFrame;

		// Wait for next frame
		float64 currentTime;
		do
		{
			currentTime = frameTimer.ElapsedProcessorTime();
		} while (currentTime < targetTimeForNextFrame);

		lastTime = currentTime;
	}

	IApplicationWindow*
	EditorClientApplication::CreateApplicationWindow(const ApplicationWindowCreateParams& a_params)
	{
		OYL_PROFILE_FUNCTION();

		IApplicationWindowFactory* factory = nullptr;

		auto* applicationModule = ModuleManager::GetModulePointer<IApplicationModule>("Application");
		if (!applicationModule->GetApplicationWindowFactories().empty())
			factory = applicationModule->GetApplicationWindowFactories()[0];

		if (!factory)
			return LogError("Failed to find valid ApplicationWindowFactory!"), nullptr;

		auto* window = factory->CreateApplicationWindow(a_params);
		if (!window)
			return LogError("Failed to create EditorClientApplication Window!"), nullptr;

		LogDebug("Created Application Window \"{}\"", window->GetTitle());

		m_applicationWindows.Add(window);
		return window;
	}

	void
	EditorClientApplication::CloseApplicationWindow(IApplicationWindow* a_window)
	{
		OYL_PROFILE_FUNCTION();

		if (!a_window || !a_window->IsValid())
			return;

		a_window->Close();

		LogDebug("Closed Application Window \"{}\"", a_window->GetTitle());

		auto iter = std::ranges::find(m_applicationWindows, a_window);
		if (iter != m_applicationWindows.end())
			m_applicationWindows.erase(iter);

		delete a_window;
	}

	void
	EditorClientApplication::RequestExit(int a_exitCode)
	{
		m_exitCode = a_exitCode;
		m_exitRequested = true;
	}

	void
	EditorClientApplication::ForceExit(int a_exitCode)
	{
		RequestExit(a_exitCode);
		::exit(a_exitCode);
	}

	bool
	EditorClientApplication::IsExitRequested(int* a_outExitCode)
	{
		if (a_outExitCode)
			*a_outExitCode = m_exitCode;
		return m_exitRequested;
	}

	uint32
	EditorClientApplication::TargetFramerate()
	{
		return m_targetFrameRate;
	}

	void
	EditorClientApplication::SetTargetFramerate(uint32 a_target)
	{
		m_targetFramerateHint = a_target;
	}

	void
	EditorClientApplication::InvokeOnUpdateCallback() const
	{
		m_onUpdateCallback.Invoke();
	}

	void
	EditorClientApplication::RegisterOnUpdateCallback(OnUpdateDelegate a_callback)
	{
		m_onUpdateCallback += a_callback;
	}

	void
	EditorClientApplication::UnregisterOnUpdateCallback(OnUpdateDelegate a_callback)
	{
		m_onUpdateCallback -= a_callback;
	}

	void
	EditorClientApplication::OnWindowCloseRequestEvent(const WindowCloseRequestEvent& a_event)
	{
		if (a_event.window != m_mainWindow)
			return;

		RequestExit(0);
	}

	void
	EditorClientApplication::OnKeyPressEvent(const WindowKeyPressEvent& a_event)
	{
		if (a_event.window != m_mainWindow)
			return;

		if (a_event.key == Input::KB_Q)
			RequestExit(1);
	}
}
