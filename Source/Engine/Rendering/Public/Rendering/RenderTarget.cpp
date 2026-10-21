#include "RenderTarget.h"

namespace Oyl
{
	RenderTarget::RenderTarget(const RenderTargetCreateParams& a_params)
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
		return m_colorAttachments.Length();
	}

	const IImage*
	RenderTarget::GetColorAttachment(uint32 a_index) const
	{
		if (a_index > m_colorAttachments.Length())
			return nullptr;

		return m_colorAttachments[a_index];
	}

	const Array<const IImage*>&
	RenderTarget::GetColorAttachments() const
	{
		return m_colorAttachments;
	}

	const IImage*
	RenderTarget::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}
}
