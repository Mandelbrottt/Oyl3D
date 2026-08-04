#pragma once

#include "Core/Math/Vector2.h"

#include "Rendering/Device.h"
#include "Rendering/SwapChain.h"

namespace Oyl::Rendering
{
	class RenderContext
	{
	protected:
		RenderContext() noexcept = default;

		DEFAULT_MOVE(RenderContext);

	public:
		NO_COPY(RenderContext);

		virtual
		~RenderContext() noexcept = default;

		virtual
		void
		Update() = 0;

		virtual
		void
		Destroy() = 0;

		virtual
		void
		Resize(Vector2u a_size) = 0;

		virtual
		const Device*
		GetDevice() const = 0;

		virtual
		SwapChain*
		GetSwapChain() = 0;

		virtual
		const SwapChain*
		GetSwapChain() const
		{
			return const_cast<RenderContext*>(this)->GetSwapChain();
		};
	};
}
