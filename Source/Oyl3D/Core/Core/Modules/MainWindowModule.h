#pragma once

#include "Module.h"

#include "Core/Application/Window.h"

namespace Oyl
{
	class MainWindowModule : Module
	{
		OYL_DECLARE_MODULE(MainWindowModule);

	public:
		void
		OnInit() override;

		void
		OnUpdate() override;

		void
		OnShutdown() override;

	private:
		Window m_window;
	};
}
