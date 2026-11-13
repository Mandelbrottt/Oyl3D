#pragma once

#include <Core/Containers/Array.h>

#include "Rendering/Hardware/Image.h"
#include "Rendering/Hardware/RenderDeviceObject.h"

namespace Oyl
{
	struct RenderTargetCreateParams
	{
		ArrayProxy<const IRdoImage*> colorImages;
		const IRdoImage* pDepthImage = nullptr;
	};

	class RENDERING_EXPORT RenderTarget : public IDestroyable
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
		const IRdoImage*
		GetColorAttachment(uint32 a_index) const;

		virtual
		const Array<const IRdoImage*>&
		GetColorAttachments() const;

		virtual
		const IRdoImage*
		GetDepthAttachment() const;

	private:
		Array<const IRdoImage*> m_colorAttachments;
		const IRdoImage* m_depthAttachment = nullptr;
	};
}
