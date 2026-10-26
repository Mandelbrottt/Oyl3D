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

		std::lock_guard lock(m_renderMutex);

		m_imGuiImpl->Update();

		// Start the Dear ImGui frame
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		if (m_onDebugGuiCallback)
			m_onDebugGuiCallback();
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
