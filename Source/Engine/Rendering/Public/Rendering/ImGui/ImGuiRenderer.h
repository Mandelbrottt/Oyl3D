#pragma once

#include <Core/Containers/SharedPtr.h>

#include "Rendering/Renderer.h"

namespace Oyl
{
	class IImGuiRenderer
	{
	public:
		virtual
		~IImGuiRenderer() = 0;

		virtual
		void
		Init(IRenderer& a_renderer) = 0;

		virtual
		void
		Update(IRenderer& a_renderer) = 0;

		virtual
		void
		Shutdown(IRenderer& a_renderer) = 0;
	};

	inline
	IImGuiRenderer::~IImGuiRenderer() {}
}
