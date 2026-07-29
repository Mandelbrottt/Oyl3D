#pragma once

#include <Core/Math/Vector2.h>

#include "DeviceObject.h"

namespace Oyl::Rendering
{
	struct ImageHandle : OpaqueHandle<ImageHandle> {};

	class Image : public IDeviceObject<ImageHandle>
	{
	protected:
		Image() = default;

		DEFAULT_MOVE(Image);

	public:
		NO_COPY(Image);

		virtual
		~Image() = default;

		virtual
		Vector2u
		GetSize() const = 0;
	};
}
