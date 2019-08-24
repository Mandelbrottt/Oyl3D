#include "oylpch.h"

#include "App/Application.h"

extern oyl::Application* oyl::createApplication();

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

#if !defined(OYL_DIST)
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
#endif

	auto app = oyl::createApplication();
	app->run();
	delete app;
}