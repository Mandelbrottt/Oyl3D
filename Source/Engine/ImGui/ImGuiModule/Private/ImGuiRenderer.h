#pragma once

#include <Core/Containers/UniquePtr.h>

#include <Rendering/ImGui/IImGuiRenderer.h>

namespace Oyl
{
	class IImGuiImpl;
	class ISwapChain;

	class ImGuiRenderer : public IImGuiRenderer
	{
	public:
		void
		Init(Renderer& a_renderer, IApplicationWindow& a_mainWindow) override;

		void
		Update() override;

		void
		Render(ICommandBuffer& a_commandBuffer) override;

		void
		Shutdown() override;

		Renderer&
		GetRenderer() const override { return *m_renderer; }

		const ImageHandle&
		GetRenderedImageHandle() const override;

		void
		SetOnUpdateCallback(Delegate<void()> a_callback) override { m_onUpdate = a_callback; }

		const ImageHandle&
		GetGameViewPortImageHandle() override { return m_gameViewPortImageHandle; }

		void
		SetGameViewPortImageHandle(const ImageHandle& a_imageHandle) override
		{
			m_gameViewPortImageHandle = a_imageHandle;
		}

	private:
		Renderer* m_renderer = nullptr;
		IApplicationWindow* m_mainWindow = nullptr;

		UniquePtr<IImGuiImpl> m_imGuiImpl;

		Delegate<void()> m_onUpdate;

		std::mutex m_renderMutex;

		ImageHandle m_gameViewPortImageHandle = nullptr;
	};
}
