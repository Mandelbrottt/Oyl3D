#pragma once

#include <Application/ApplicationWindow.h>

#include "Rendering/Image.h"

namespace Oyl
{
	class Renderer;
	class ICommandBuffer;

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
		Render(ICommandBuffer& a_commandBuffer) = 0;

		virtual
		void
		Shutdown() = 0;

		virtual
		Renderer&
		GetRenderer() const = 0;

		virtual
		const ImageHandle&
		GetRenderedImageHandle() const = 0;

		virtual
		void
		SetOnUpdateCallback(Delegate<void()> a_callback) = 0;

		virtual
		const ImageHandle&
		GetGameViewPortImageHandle() = 0;

		virtual
		void
		SetGameViewPortImageHandle(const ImageHandle& a_imageHandle) = 0;
	};

	inline
	IImGuiRenderer::~IImGuiRenderer() {}
}
