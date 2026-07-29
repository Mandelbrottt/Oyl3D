#pragma once

#include "DeviceObject.h"
#include "Image.h"

namespace Oyl::Rendering
{
	class RenderTarget : public IUniqueHandle
	{
	public:
		RenderTarget() = default;

		struct CreateParams
		{
			const Image** pColorImages = nullptr;
			uint32 colorImageLength = 0;

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
		std::vector<const Image*>
		GetColorAttachments() const;

		virtual
		const Image*
		GetDepthAttachment() const;

	private:
		std::vector<const Image*> m_colorAttachments;
		const Image* m_depthAttachment = nullptr;
	};
}
