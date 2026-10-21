#pragma once

#include <Application/Application.h>

#include <Core/Containers/Delegate.h>
#include <Core/Containers/SharedPtr.h>

#include <Rendering/Renderer.h>
#include <Rendering/TestRenderPass.h>

namespace Oyl
{
	class EditorClientApplication : public IApplication
	{
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

		void
		InvokeOnUpdateCallback() const override;

		void
		RegisterOnUpdateCallback(OnUpdateDelegate a_callback) override;

		void
		UnregisterOnUpdateCallback(OnUpdateDelegate a_callback) override;

	private:
		void
		OnWindowCloseRequestEvent(const WindowCloseRequestEvent& a_event);

		void
		OnKeyPressEvent(const WindowKeyPressEvent& a_event);

	private:
		IApplicationWindow* m_mainWindow = nullptr;

		RendererProcessor m_rendererProcessor;
		UniquePtr<TestRenderPass> m_testRenderPass;

		bool m_exitRequested = false;
		int m_exitCode = 0;
		uint32 m_targetFrameRate = 0;
		std::atomic<uint32> m_targetFramerateHint = 0;

		Array<IApplicationWindow*> m_applicationWindows;
		MulticastDelegate<void()> m_onUpdateCallback;
	};
}
