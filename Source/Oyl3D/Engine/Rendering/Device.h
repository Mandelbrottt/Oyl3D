#pragma once

#include "Rendering/Window.h"

namespace Oyl::Rendering
{
	class IDevice : public IUniqueHandle
	{
	protected:
		IDevice() = default;

		IDevice(IDevice&& a_other) noexcept = default;
		IDevice&
		operator =(IDevice&& a_other) noexcept = default;

	public:
		virtual
		~IDevice() = default;

		void
		Destroy() override = 0;

		bool
		IsValid() const override = 0;

		virtual
		const IWindow*
		GetWindow() const = 0;
	};
}
