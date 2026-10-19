#include "RenderTarget.h"

namespace Oyl
{
	IRenderTarget::IRenderTarget(const RenderTargetCreateParams& a_params)
	{
		m_colorAttachments = a_params.colorImages;
		m_depthAttachment = a_params.pDepthImage;
	}

	IRenderTarget::~IRenderTarget()
	{
		IRenderTarget::Destroy();
	}

	void
	IRenderTarget::Destroy()
	{
		m_colorAttachments.Clear();
		m_depthAttachment = nullptr;
	}

	bool
	IRenderTarget::IsValid() const
	{
		return !m_colorAttachments.Empty() || m_depthAttachment;
	}

	uint32
	IRenderTarget::GetNumColorAttachments() const
	{
		return m_colorAttachments.Length();
	}

	const IImage*
	IRenderTarget::GetColorAttachment(uint32 a_index) const
	{
		if (a_index > m_colorAttachments.Length())
			return nullptr;

		return m_colorAttachments[a_index];
	}

	const Array<const IImage*>&
	IRenderTarget::GetColorAttachments() const
	{
		return m_colorAttachments;
	}

	const IImage*
	IRenderTarget::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}
}
