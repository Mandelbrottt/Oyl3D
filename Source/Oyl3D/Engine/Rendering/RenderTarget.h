#pragma once

#include <Core/Array.h>

#include "DeviceObject.h"
#include "Image.h"

namespace Oyl::Rendering
{
	class RenderTarget : public IUniqueHandle
	{
	public:
		RenderTarget(nullptr_t) {}

		struct CreateParams
		{
			ArrayProxy<const ImageImpl*> colorImages;
			const ImageImpl* pDepthImage = nullptr;
		};

		explicit
		RenderTarget(const CreateParams& a_params);

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
		const ImageImpl*
		GetColorAttachment(uint32 a_index) const;

		virtual
		const Array<const ImageImpl*>&
		GetColorAttachments() const;

		virtual
		const ImageImpl*
		GetDepthAttachment() const;

	private:
		Array<const ImageImpl*> m_colorAttachments;
		const ImageImpl* m_depthAttachment = nullptr;
	};
}
