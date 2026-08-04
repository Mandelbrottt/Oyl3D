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
			ArrayProxy<const Image*> colorImages;
			const Image* pDepthImage = nullptr;
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
		const Image*
		GetColorAttachment(uint32 a_index) const;

		virtual
		const Array<const Image*>&
		GetColorAttachments() const;

		virtual
		const Image*
		GetDepthAttachment() const;

	private:
		Array<const Image*> m_colorAttachments;
		const Image* m_depthAttachment = nullptr;
	};
}
