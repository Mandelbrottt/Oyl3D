#pragma once

namespace Oyl
{
	class IRenderer
	{
	public:
		virtual
		~IRenderer() = 0;

		virtual
		void
		Init() = 0;

		virtual
		void
		Shutdown() = 0;

		virtual
		void
		RegisterApplicationWindow(IApplicationWindow* a_window) = 0;

		virtual
		void
		UnregisterApplicationWindow(IApplicationWindow* a_window) = 0;
	};

	inline
	IRenderer::~IRenderer() {}
}
