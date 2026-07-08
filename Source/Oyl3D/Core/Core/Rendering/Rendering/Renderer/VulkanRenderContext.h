#pragma once

#include "RenderContext.h"

namespace Oyl::Rendering::Internal
{
	class OYL_RENDERING_API VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext() noexcept;

		explicit
		VulkanRenderContext(const RenderContextParams& a_params) noexcept;

		VulkanRenderContext(VulkanRenderContext&& a_other) noexcept;
		VulkanRenderContext&
		operator=(VulkanRenderContext&& a_other) noexcept;

		virtual
		~VulkanRenderContext();

		void
		Init(const RenderContextParams& a_params) override;

		void
		Update() override;

		void
		Destroy() override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}