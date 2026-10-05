#include "MainLoop.h"

#include <Windows.h>

#include <Core/Application/Application.h>

namespace Oyl
{
	EditorClientMainLoop::EditorClientMainLoop() {}

	EditorClientMainLoop::~EditorClientMainLoop() {}

	void
	EditorClientMainLoop::Update()
	{
		if (GetKeyState('R'))
			Application::RequestExit();
	}
}
