#pragma once
#include <Core/Math/Vector2.h>

namespace Oyl::Rendering
{
	class OYL_RENDERING_API SwapChain
	{
	protected:
		SwapChain();

	public:
		SwapChain(const SwapChain&) = delete;
		SwapChain&
		operator =(const SwapChain&) = delete;

		SwapChain(SwapChain&& a_other) noexcept = default;
		SwapChain&
		operator =(SwapChain&& a_other) noexcept = default;

		virtual
		~SwapChain();

		virtual
		bool
		Destroy()
		{
			return true;
		}

		virtual
		void
		Recreate() = 0;

		virtual
		bool
		IsValid() const = 0;

		explicit
		operator bool() const
		{
			return IsValid();
		}
	};
}
