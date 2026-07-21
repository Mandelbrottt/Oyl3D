#pragma once

#include "RenderContext.h"

namespace Oyl::Rendering::Vulkan
{
	class OYL_RENDERING_API RenderContext final : public Rendering::RenderContext
	{
	public:
		RenderContext() noexcept;

		explicit
		RenderContext(const RenderContextParams& a_params) noexcept;

		RenderContext(RenderContext&& a_other) noexcept;
		RenderContext&
		operator=(RenderContext&& a_other) noexcept;

		virtual
		~RenderContext();

		void
		Init(const RenderContextParams& a_params) override;

		void
		Update() override;

		void
		Destroy() override;

		void
		Resize(Vector2i a_size) override;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}