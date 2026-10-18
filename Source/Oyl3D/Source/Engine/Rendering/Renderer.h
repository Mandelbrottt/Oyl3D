#pragma once

#include <Application/ApplicationWindow.h>

namespace Oyl
{
	class IRenderer
	{
	public:
		virtual
		~IRenderer() = 0;

		virtual
		void
		Init(IApplicationWindow* a_mainWindow) = 0;

		virtual
		void
		Update() = 0;

		virtual
		void
		Shutdown() = 0;

		virtual
		void
		RegisterApplicationWindow(IApplicationWindow& a_window) = 0;

		virtual
		void
		UnregisterApplicationWindow(IApplicationWindow& a_window) = 0;
	};

	inline
	IRenderer::~IRenderer() {}
}
