#pragma once

#include <Application/ApplicationWindow.h>

#include "Rendering/Hardware/RdoImage.h"

namespace Oyl
{
	class Renderer;
	class IRdoCommandBuffer;

	class IImGuiRenderer
	{
	public:
		virtual
		~IImGuiRenderer() = 0;

		virtual
		void
		Init(Renderer& a_renderer, IApplicationWindow& a_mainWindow) = 0;

		virtual
		void
		Update() = 0;

		virtual
		void
		Render(IRdoCommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		Shutdown() = 0;

		virtual
		Renderer&
		GetRenderer() const = 0;

		virtual
		const RdoImageHandle&
		GetRenderedImageHandle() const = 0;

		virtual
		const MulticastDelegate<void()>&
		OnDebugGuiCallback() const = 0;

		virtual
		const RdoImageHandle&
		GetGameViewPortImageHandle() = 0;

		virtual
		void
		SetGameViewPortImageHandle(const RdoImageHandle& a_imageHandle) = 0;
	};

	inline
	IImGuiRenderer::~IImGuiRenderer() {}
}
