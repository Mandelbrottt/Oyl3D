#pragma once

#include "RenderPass.h"

namespace Oyl
{
	class IColorOutputRenderPass
	{
	public:
		virtual
		~IColorOutputRenderPass() = 0;

		virtual
		ImageHandle
		GetColorAttachment() const = 0;
	};

	inline
	IColorOutputRenderPass::~IColorOutputRenderPass() {}
}
