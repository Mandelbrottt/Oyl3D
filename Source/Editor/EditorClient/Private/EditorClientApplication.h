#pragma once

#include <Application/Application.h>

#include <Core/Containers/Delegate.h>
#include <Core/Containers/SharedPtr.h>

#include <Rendering/RenderContext.h>
#include <Rendering/Renderer.h>
#include <Rendering/TestRenderPass.h>

namespace Oyl
{
	class EditorClientApplication : public IApplication
	{
		void
		Start() override;

		void
		Shutdown() override;

		void
		Update() override;

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

		const MulticastDelegate<void()>&
		OnUpdateCallback() const override { return m_onUpdateCallback; }

		const MulticastDelegate<void()>&
		OnDebugGuiCallback() const override { return m_renderer.OnDebugGuiCallback(); }

		void
		OnDebugGui();

	private:
		void
		OnWindowCloseRequestEvent(const WindowCloseRequestEvent& a_event);

		void
		OnKeyPressEvent(const WindowKeyPressEvent& a_event);

	private:
		IApplicationWindow* m_mainWindow = nullptr;

		Renderer m_renderer;
		UniquePtr<TestRenderPass> m_testRenderPass;

		bool m_exitRequested = false;
		int m_exitCode = 0;
		uint32 m_targetFrameRate = 0;
		std::atomic<uint32> m_targetFramerateHint = 0;

		Array<IApplicationWindow*> m_applicationWindows;
		MulticastDelegate<void()> m_onUpdateCallback;
	};
}
