#pragma once

#include "RenderContext.h"

namespace Oyl::Rendering
{
	class OYL_CORE_API VulkanRenderContext final : public RenderContext
	{
	public:
		VulkanRenderContext() noexcept;

		explicit
		VulkanRenderContext(const RenderContextParams& a_params) noexcept;

		VulkanRenderContext(VulkanRenderContext&& a_other) noexcept;
		VulkanRenderContext&
		operator=(VulkanRenderContext&& a_other) noexcept;

		void
		Init(const RenderContextParams& a_params) override;

		void
		Update() override;

		void
		Destroy() override;

		void
		CreateInstance();

	private:
		void
		SetupDebugMessenger();

		std::vector<const char*>
		GetRequiredInstanceExtensions() const;

	private:
		struct Impl;
		Impl* m_impl;
	};
}