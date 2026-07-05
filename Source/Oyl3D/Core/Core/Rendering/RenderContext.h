#pragma once

namespace Oyl
{
	class Window;
}

namespace Oyl::Rendering
{
	struct RenderContextParams
	{
		Window* window;
	};

	class OYL_CORE_API RenderContext
	{
	protected:
		RenderContext() noexcept;

		explicit
		RenderContext(const RenderContextParams& a_params) noexcept;

		RenderContext(RenderContext&& a_other) noexcept = default;
		RenderContext&
		operator=(RenderContext&& a_other) noexcept = default;

	public:
		RenderContext(const RenderContext&) = delete;
		RenderContext&
		operator=(const RenderContext&) = delete;

		virtual
		~RenderContext() noexcept;

		virtual
		void
		Init(const RenderContextParams& a_params) = 0;

		virtual
		void
		Update() = 0;

		virtual
		void
		Destroy() = 0;
	};
}