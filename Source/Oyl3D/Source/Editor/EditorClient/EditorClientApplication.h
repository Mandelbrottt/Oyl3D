#pragma once

#include <Application/Application.h>

#include <Core/Events/Event.h>

namespace Oyl
{
	class IApplicationWindow;

	class EditorClientApplication : public IApplication
	{
	private:
		void
		OnStart() override;

		void
		OnStop() override;

		void
		OnUpdate() override;

		void
		WaitIdle() override;

	public:
		const Array<IApplicationWindow*>&
		GetApplicationWindows() const override { return m_applicationWindows; }

		IApplicationWindow*
		CreateApplicationWindow(const ApplicationWindowCreateParams& a_params) override;

		void
		CloseApplicationWindow(IApplicationWindow* a_window) override;

		void
		RequestExit(int a_exitCode) override;

		void
		ForceExit(int a_exitCode) override;

		bool
		IsExitRequested(int* a_outExitCode) override;

		uint32
		TargetFramerate() override;

		void
		SetTargetFramerate(uint32 a_target) override;

		const Array<OnUpdateDelegate>&
		GetOnUpdateCallbacks() const override;

		void
		RegisterOnUpdateCallback(OnUpdateDelegate a_callback) override;

		void
		UnregisterOnUpdateCallback(OnUpdateDelegate a_callback) override;

	private:
		virtual
		void
		PostEvent(const Event& a_event);

	private:
		IApplicationWindow* m_mainWindow = nullptr;

		bool m_exitRequested = false;
		int m_exitCode = 0;
		uint32 m_targetFrameRate = 0;
		std::atomic<uint32> m_targetFramerateHint = 0;

		Array<IApplicationWindow*> m_applicationWindows;
		Array<OnUpdateDelegate> m_onUpdateCallbacks;
	};
}
