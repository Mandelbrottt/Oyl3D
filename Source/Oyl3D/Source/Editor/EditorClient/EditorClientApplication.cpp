#include "EditorClientApplication.h"

#include <Application/Application.h>

#include <Core/Logging/Logging.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Time/Time.h>

namespace Oyl
{
	void
	EditorClientApplication::OnStart()
	{
		ModuleLoadResult loadResult;
		auto applicationModule = ModuleManager::LoadModulePointer("Application", &loadResult);

		if (!applicationModule)
			LogError("Failed to load ApplicationModule with error \"{}\"!",
			         Traits::TUnderlyingType<decltype(loadResult)>(loadResult));

		m_mainWindow = CreateApplicationWindow({
			.title = "Oyl3D - Now with more DLLs!",
			.size = Vector2u(1280, 720),
			.postEventCallback = PostEventDelegate::Create(this, &EditorClientApplication::PostEvent)
		});
	}

	void
	EditorClientApplication::OnStop()
	{
		CloseApplicationWindow(m_mainWindow);
		m_mainWindow = nullptr;
	}

	void
	EditorClientApplication::OnUpdate()
	{
		m_targetFrameRate = m_targetFramerateHint;

		if (m_mainWindow && m_mainWindow->IsValid())
			m_mainWindow->Update();
	}

	void
	EditorClientApplication::WaitIdle()
	{
		auto* frameTimer = Time::Internal::GetFrameTimer();

		// Intentionally use g_frameTimer instead of Oyl::Time::GetFrameTimer since that is user overrideable
		static auto lastTick = frameTimer->ElapsedProcessorTicks();

		// If target framerate is 0, don't cap framerate
		uint32 targetFrameRate = TargetFramerate();
		uint64 minNumTicksPerFrame = 0;
		if (targetFrameRate > 0)
			minNumTicksPerFrame = (uint64) (frameTimer->TickResolution() * (1.0 / targetFrameRate));

		auto targetTickForNextFrame = lastTick + minNumTicksPerFrame;

		// Wait for next frame
		uint64 thisTick;
		do
		{
			thisTick = frameTimer->ElapsedProcessorTicks();
		} while (thisTick < targetTickForNextFrame);

		lastTick = thisTick;
	}

	IApplicationWindow*
	EditorClientApplication::CreateApplicationWindow(const ApplicationWindowCreateParams& a_params)
	{
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

	const Array<OnUpdateDelegate>&
	EditorClientApplication::GetOnUpdateCallbacks() const
	{
		return m_onUpdateCallbacks;
	}

	void
	EditorClientApplication::RegisterOnUpdateCallback(OnUpdateDelegate a_callback)
	{
		if (!a_callback.IsValid())
			return;

		for (const auto& callback : m_onUpdateCallbacks)
			if (callback == a_callback)
				return;

		m_onUpdateCallbacks.Add(a_callback);
	}

	void
	EditorClientApplication::UnregisterOnUpdateCallback(OnUpdateDelegate a_callback)
	{
		if (!a_callback.IsValid())
			return;

		for (uint i = 0; i < m_onUpdateCallbacks.Length(); i++)
			if (m_onUpdateCallbacks[i] == a_callback)
				m_onUpdateCallbacks.erase(m_onUpdateCallbacks.begin() + i);
	}

	void
	EditorClientApplication::PostEvent(const Event& a_event)
	{
		if (a_event.eventType == typeidof(WindowCloseRequestEvent))
			RequestExit(0);

		if (a_event.eventType == typeidof(WindowKeyPressEvent))
		{
			const auto& evt = static_cast<const WindowKeyPressEvent&>(a_event);
			if (evt.key == Input::KeyboardKey::KB_Q)
				RequestExit(1);
		}
	}
}
