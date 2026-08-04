#include "RenderTarget.h"

namespace Oyl::Rendering
{
	RenderTarget::RenderTarget(const CreateParams& a_params)
	{
		m_colorAttachments = a_params.colorImages;
		m_depthAttachment = a_params.pDepthImage;
	}

	RenderTarget::~RenderTarget()
	{
		RenderTarget::Destroy();
	}

	void
	RenderTarget::Destroy()
	{
		m_colorAttachments.Clear();
		m_depthAttachment = nullptr;
	}

	bool
	RenderTarget::IsValid() const
	{
		return !m_colorAttachments.Empty() || m_depthAttachment;
	}

	uint32
	RenderTarget::GetNumColorAttachments() const
	{
		return m_colorAttachments.Size();
	}

	const ImageImpl*
	RenderTarget::GetColorAttachment(uint32 a_index) const
	{
		if (a_index > m_colorAttachments.Size())
			return nullptr;

		return m_colorAttachments[a_index];
	}

	const Array<const ImageImpl*>&
	RenderTarget::GetColorAttachments() const
	{
		return m_colorAttachments;
	}

	const ImageImpl*
	RenderTarget::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}
}
