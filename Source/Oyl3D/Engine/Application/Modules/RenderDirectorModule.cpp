#include "RenderDirectorModule.h"

namespace Oyl
{
	bool
	RenderDirectorModule::IsEnabled()
	{
		return Module::IsEnabled();
	}

	void
	RenderDirectorModule::Setup()
	{
		RegisterEventListener(&RenderDirectorModule::OnWindowCreatedEvent);
		RegisterEventListener(&RenderDirectorModule::OnWindowClosedEvent);
		RegisterEventListener(&RenderDirectorModule::OnWindowResizeEvent);
		RegisterEventListener(&RenderDirectorModule::OnWindowMinimizeEvent);
	}

	void
	RenderDirectorModule::Init()
	{
		Module::Init();
	}

	void
	RenderDirectorModule::Update()
	{
		Module::Update();
	}

	void
	RenderDirectorModule::Shutdown()
	{
		Module::Shutdown();
	}

	void
	RenderDirectorModule::OnWindowCreatedEvent(const WindowCreatedEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	RenderDirectorModule::OnWindowClosedEvent(const WindowClosedEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	RenderDirectorModule::OnWindowResizeEvent(const WindowResizeEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}

	void
	RenderDirectorModule::OnWindowMinimizeEvent(const WindowMinimizeEvent& a_event)
	{
		OYL_UNUSED(a_event);
	}
}
