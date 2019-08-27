#include "oylpch.h"

#include "App/Application.h"

extern oyl::Application* oyl::createApplication();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

#if defined(OYL_LOG_CONSOLE)
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	auto app = oyl::createApplication();
	app->run();
	delete app;
}