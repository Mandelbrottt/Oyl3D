#include "RenderTarget.h"

namespace Oyl::Rendering
{
	RenderTarget::RenderTarget(const CreateParams& a_params)
	{
		if (a_params.pColorImages && a_params.colorImageLength)
		{
			m_colorAttachments.reserve(a_params.colorImageLength);
			m_colorAttachments.insert(
				m_colorAttachments.end(),
				&a_params.pColorImages[0],
				&a_params.pColorImages[a_params.colorImageLength]
			);
		}

		if (a_params.pDepthImage)
		{
			m_depthAttachment = a_params.pDepthImage;
		}
	}

	RenderTarget::~RenderTarget()
	{
		RenderTarget::Destroy();
	}

	void
	RenderTarget::Destroy()
	{
		m_colorAttachments.clear();
		m_depthAttachment = nullptr;
	}

	bool
	RenderTarget::IsValid() const
	{
		return !m_colorAttachments.empty() || m_depthAttachment;
	}

	uint32
	RenderTarget::GetNumColorAttachments() const
	{
		return (uint32) m_colorAttachments.size();
	}

	const Image*
	RenderTarget::GetColorAttachment(uint32 a_index) const
	{
		if (a_index > m_colorAttachments.size())
			return nullptr;

		return m_colorAttachments[a_index];
	}

	std::vector<const Image*>
	RenderTarget::GetColorAttachments() const
	{
		return m_colorAttachments;
	}

	const Image*
	RenderTarget::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}
}
