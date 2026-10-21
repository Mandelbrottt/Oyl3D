#pragma once

#include <Core/Containers/Array.h>

#include "IDeviceObject.h"
#include "Image.h"

namespace Oyl
{
	struct RenderTargetCreateParams
	{
		ArrayProxy<const IImage*> colorImages;
		const IImage* pDepthImage = nullptr;
	};

	class RenderTarget : public IDestroyable
	{
	public:
		RenderTarget() = default;

		explicit
		RenderTarget(const RenderTargetCreateParams& a_params);

		DEFAULT_MOVE(RenderTarget);
		NO_COPY(RenderTarget);

		virtual
		~RenderTarget();

		void
		Destroy() override;

		bool
		IsValid() const override;

		virtual
		uint32
		GetNumColorAttachments() const;

		virtual
		const IImage*
		GetColorAttachment(uint32 a_index) const;

		virtual
		const Array<const IImage*>&
		GetColorAttachments() const;

		virtual
		const IImage*
		GetDepthAttachment() const;

	private:
		Array<const IImage*> m_colorAttachments;
		const IImage* m_depthAttachment = nullptr;
	};
}
