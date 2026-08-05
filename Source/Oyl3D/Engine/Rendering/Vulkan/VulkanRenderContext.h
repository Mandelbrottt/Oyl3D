#pragma once

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include "Rendering/RenderContext.h"

namespace Oyl::Rendering
{
	class OYL_RENDERING_API VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext() noexcept;

		struct CreateParams
		{
			const IWindow* window;
		};

		explicit
		VulkanRenderContext(const CreateParams& a_params) noexcept;

		NO_COPY(VulkanRenderContext);

		VulkanRenderContext(VulkanRenderContext&& a_other) noexcept;
		VulkanRenderContext&
		operator=(VulkanRenderContext&& a_other) noexcept;

		virtual
		~VulkanRenderContext();

		void
		Init(const CreateParams& a_params);

		void
		Update() override;

		void
		Destroy() override;

		void
		Resize(Vector2u a_size) override;

		const VulkanDevice*
		GetDevice() const override;

		VulkanSwapChain*
		GetSwapChain() override;

		const VulkanSwapChain*
		GetSwapChain() const override
		{
			return const_cast<VulkanRenderContext*>(this)->GetSwapChain();
		}

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
