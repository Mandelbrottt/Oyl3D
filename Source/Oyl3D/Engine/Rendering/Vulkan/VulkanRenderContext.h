#pragma once

#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

#include "Rendering/RenderContext.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API RenderContext final : public IRenderContext
	{
	public:
		RenderContext() noexcept;

		struct CreateParams
		{
			const IWindow* window;
		};

		explicit
		RenderContext(const CreateParams& a_params) noexcept;

		NO_COPY(RenderContext);

		RenderContext(RenderContext&& a_other) noexcept;
		RenderContext&
		operator=(RenderContext&& a_other) noexcept;

		virtual
		~RenderContext();

		void
		Init(const CreateParams& a_params);

		void
		Update() override;

		void
		Destroy() override;

		void
		Resize(Vector2i a_size) override;

		const Device*
		GetDevice() const override;

		const SwapChain*
		GetSwapChain() const override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
