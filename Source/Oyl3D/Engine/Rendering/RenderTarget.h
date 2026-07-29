#pragma once

#include "DeviceObject.h"
#include "Image.h"

namespace Oyl::Rendering
{
	struct RenderTargetHandle : OpaqueHandle<RenderTargetHandle> {};

	class RenderTarget : public IUniqueHandle
	{
	protected:
		RenderTarget() = default;

		DEFAULT_MOVE(RenderTarget);

	public:
		NO_COPY(RenderTarget);

		virtual
		~RenderTarget() = default;

		virtual
		uint32
		GetNumColorAttachments() const;

		virtual
		const Image&
		GetColorAttachment(uint32 a_index) const;

		virtual
		std::vector<const Image&>
		GetColorAttachments() const;

		virtual
		const Image*
		GetDepthAttachment() const;
	};
}
