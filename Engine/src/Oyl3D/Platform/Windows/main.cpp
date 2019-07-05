#include "oylpch.h"

#ifdef OYL_PLATFORM_WINDOWS

#include "App/Application.h"

#include <windows.h>

extern oyl::Application* oyl::createApplication();

int main(int argc, char** argv) {
	auto app = oyl::createApplication();
	app->run();
	delete app;
}

//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
//{
//	try
//	{
//		int argc;
//		LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
//
//		if (argv) LocalFree(argv);
//		return EXIT_SUCCESS;
//	}
//	catch (const std::exception& e)
//	{
//		return EXIT_FAILURE;
//	}
//}

#endif