#pragma once

#include <Core/Array.h>
#include <Core/PImpl.h>
#include <Core/Math/Vector2.h>

#include "DeviceObject.h"
#include "Enums.h"

namespace Oyl::Rendering
{
	class CommandBuffer;

	struct ImageId : OpaqueHandle<ImageId> {};

	class Image : public IDeviceObject<ImageId>
	{
	protected:
		Image() = default;

	public:
		struct CreateParams
		{
			ArrayProxy<byte> pixelData = {};

			Vector2u size;

			ImageFormat format;
			ImageUsageFlags usageFlags;
			ImageLayout layout;
		};

		NO_MOVE(Image);
		NO_COPY(Image);

		virtual
		~Image() = default;

		virtual
		Vector2u
		GetSize() const = 0;

		virtual
		ImageFormat
		GetFormat() const = 0;

		virtual
		ImageUsageFlags
		GetUsageFlags() const = 0;

		virtual
		ImageLayout
		GetLayout() const = 0;
	};

	using ImageHandle = PImpl<Image>;
}
