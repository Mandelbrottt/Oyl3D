#include "ImGuiRenderer.h"

#include <Core/Modules/ModuleManager.h>

#include "ImGuiModule/IImGuiImpl.h"
#include "ImGuiModule/ImGuiModule.h"

#include "Rendering/Renderer.h"

namespace Oyl
{
	void
	ImGuiRenderer::Init(Renderer& a_renderer, IApplicationWindow& a_mainWindow)
	{
		OYL_PROFILE_FUNCTION();

		m_renderer = &a_renderer;
		m_mainWindow = &a_mainWindow;

		auto* imGuiModule = ModuleManager::GetModulePointer<IImGuiModule>("ImGuiModule");
		if (imGuiModule)
			m_imGuiImpl = imGuiModule->CreateImGuiImpl(m_renderer->GetRenderContext().GetRenderingApi());
		OYL_ASSERT(m_imGuiImpl);

		m_imGuiImpl->Init(*this, *m_mainWindow);
	}

	void
	ImGuiRenderer::Update()
	{
		OYL_PROFILE_FUNCTION();

		OYL_ASSERT(m_mainWindow);

		static bool show_demo_window = true;
		static bool show_another_window = false;
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		auto& io = ImGui::GetIO();

		std::lock_guard lock(m_renderMutex);

		m_imGuiImpl->Update();

		// Start the Dear ImGui frame
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		if (m_onUpdate)
			m_onUpdate();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*) &clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}
	}

	void
	ImGuiRenderer::Render(ICommandBuffer& a_commandBuffer)
	{
		OYL_PROFILE_FUNCTION();

		std::lock_guard lock(m_renderMutex);

		m_imGuiImpl->Render(a_commandBuffer);

		auto& io = ImGui::GetIO();
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void
	ImGuiRenderer::Shutdown()
	{
		OYL_PROFILE_FUNCTION();

		m_imGuiImpl->Shutdown();

		m_gameViewPortImageHandle.Reset();
	}

	const ImageHandle&
	ImGuiRenderer::GetRenderedImageHandle() const
	{
		return m_imGuiImpl->GetRenderedImageHandle();
	}
}
