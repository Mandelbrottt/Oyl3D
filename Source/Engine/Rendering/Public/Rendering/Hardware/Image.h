#pragma once

#include <Core/Containers/Array.h>
#include <Core/Math/Vector2.h>

#include "RenderDeviceObject.h"

#include "Rendering/RenderingEnums.h"

namespace Oyl
{
	struct RdoImageId : IRenderDeviceObjectId<RdoImageId> {};

	struct RdoImageCreateParams
	{
		ArrayProxy<byte> pixelData = {};

		Vector2u size;

		ImageFormat format;
		ImageUsageFlags usageFlags;
		ImageLayout layout;
	};

	class IRdoImage : public IRenderDeviceObject<RdoImageId>
	{
	protected:
		IRdoImage() = default;

		DEFAULT_MOVE(IRdoImage);

	public:
		NO_COPY(IRdoImage);

		virtual
		~IRdoImage() = 0;

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
	IRdoImage::~IRdoImage() {}

	using RdoImageHandle = IRenderDeviceObjectHandle<IRdoImage>;
}
