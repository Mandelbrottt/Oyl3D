#pragma once

#include <Core/Modules/Module.h>

namespace Oyl
{
	struct WindowMinimizeEvent;
	struct WindowResizeEvent;
	struct WindowClosedEvent;
	struct WindowCreatedEvent;

	class RenderDirectorModule : public Module
	{
		OYL_DECLARE_MODULE(RenderDirectorModule, Module);

	public:
		bool
		IsEnabled() override;

		void
		Setup() override;

		void
		Init() override;

		void
		Update() override;

		void
		Shutdown() override;

	private:
		void
		OnWindowCreatedEvent(const WindowCreatedEvent& a_event);

		void
		OnWindowClosedEvent(const WindowClosedEvent& a_event);

		void
		OnWindowResizeEvent(const WindowResizeEvent& a_event);

		void
		OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event);

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
