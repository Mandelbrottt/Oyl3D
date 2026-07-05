#include "RenderContext.h"

namespace Oyl::Rendering
{
	RenderContext::RenderContext() noexcept {}

	RenderContext::RenderContext(const RenderContextParams& a_params) noexcept
		: RenderContext()
	{
		OYL_UNUSED(a_params);
	}

	RenderContext::~RenderContext() noexcept {}
}
