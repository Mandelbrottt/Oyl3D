#pragma once

#include "Core/Math/Vector2.h"

namespace Oyl
{
	class Window;

	namespace Internal
	{
		class ResourceManager;
	}
}

namespace Oyl::Rendering
{
	class Device;
	class SwapChain;

	class OYL_RENDERING_API RenderContext
	{
	protected:
		RenderContext() noexcept;

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
		Update() = 0;

		virtual
		void
		Destroy() = 0;

		virtual
		void
		Resize(Vector2i a_size) = 0;

		virtual
		const Device*
		GetDevice() const = 0;

		virtual
		const SwapChain*
		GetSwapChain() const = 0;
	};
}
