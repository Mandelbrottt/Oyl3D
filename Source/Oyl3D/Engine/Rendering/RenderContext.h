#pragma once

#include "Core/Math/Vector2.h"

#include "Rendering/Device.h"
#include "Rendering/SwapChain.h"

namespace Oyl::Rendering
{
	class IRenderContext
	{
	protected:
		IRenderContext() noexcept = default;

		DEFAULT_MOVE(IRenderContext);

	public:
		NO_COPY(IRenderContext);

		virtual
		~IRenderContext() noexcept = default;

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
		const DeviceImpl*
		GetDevice() const = 0;

		virtual
		const ISwapChain*
		GetSwapChain() const = 0;
	};
}
