#pragma once

namespace Oyl
{
	class IRenderer
	{
	public:
		virtual
		~IRenderer() = 0;
	};

	inline
	IRenderer::~IRenderer() {}
}
