#pragma once

#include "VulkanDevice.h"

#include "Rendering/RenderContext.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API RenderContext final : public Rendering::RenderContext
	{
	public:
		RenderContext() noexcept;

		struct CreateParams
		{
			const Window* window;
		};

		explicit
		RenderContext(const CreateParams& a_params) noexcept;

		RenderContext(const RenderContext&) = delete;
		RenderContext&
		operator=(const RenderContext&) = delete;

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

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
