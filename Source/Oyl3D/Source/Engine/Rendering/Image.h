#pragma once

#include <Core/Containers/Array.h>
#include <Core/Math/Vector2.h>

#include "IDeviceObject.h"
#include "RenderingEnums.h"

namespace Oyl
{
	struct ImageId : IDeviceObjectId<ImageId> {};

	struct ImageCreateParams
	{
		ArrayProxy<byte> pixelData = {};

		Vector2u size;

		ImageFormat format;
		ImageUsageFlags usageFlags;
		ImageLayout layout;
	};

	class Image : public IDeviceObject<ImageId>
	{
	protected:
		Image() = default;

		DEFAULT_MOVE(Image);

	public:
		NO_COPY(Image);

		virtual
		~Image() = 0;

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

	inline
	Image::~Image() {}
}
