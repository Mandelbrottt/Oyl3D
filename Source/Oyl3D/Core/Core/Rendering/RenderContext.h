#pragma once

namespace Oyl::Rendering
{
	struct RenderContextParams
	{

	};

	class RenderContext
	{
	public:
		RenderContext() noexcept;

		explicit
		RenderContext(const RenderContextParams& a_params) noexcept;

		RenderContext(const RenderContext&) = delete;
		RenderContext&
		operator=(const RenderContext&) = delete;

		RenderContext(RenderContext&& a_other) noexcept;
		RenderContext&
		operator=(RenderContext&& a_other) noexcept;

		~RenderContext() noexcept;

		void
		Init(const RenderContextParams& a_params);

		void
		Update();

		void
		Destroy();

	private:
		struct Impl;
		Impl* m_impl;
	};
}