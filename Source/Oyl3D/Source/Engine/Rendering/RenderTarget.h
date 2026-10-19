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

	class IRenderTarget : public IDestroyable
	{
	public:
		IRenderTarget(nullptr_t) {}

		explicit
		IRenderTarget(const RenderTargetCreateParams& a_params);

		DEFAULT_MOVE(IRenderTarget);
		NO_COPY(IRenderTarget);

		virtual
		~IRenderTarget();

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
