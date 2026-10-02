#include <iostream>

#include <Core/Logging/Logging.h>
#include <Core/Module/ModuleManager.h>

int
main(int a_argc, char** a_argv)
{
	(void) a_argc;
	(void) a_argv;
	puts("Hello World!");

	Oyl::ModuleManager::LoadModulePointer("Core");
	Oyl::ModuleManager::LoadModulePointer("SpdLogLogger");

	Oyl::ModuleManager::LoadModulePointer("Test");

	Oyl::Log("Testing {} {} {}", 1, 2, 3);

	Oyl::Internal::g_currentLogger->Flush();

	Oyl::ModuleManager::Get().UnloadModule("Test");

	Oyl::ModuleManager::Get().UnloadModule("SpdLogLogger");
	Oyl::ModuleManager::Get().UnloadModule("Core");
}

#if defined(_WIN32)
#include <Windows.h>

int
WINAPI
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();

	FILE* inFileStream;
	FILE* outFileStream;
	FILE* errFileStream;

	// Direct input and output to the console
	freopen_s(&inFileStream, "CONIN$", "r", stdin);
	freopen_s(&outFileStream, "CONOUT$", "w", stdout);
	freopen_s(&errFileStream, "CONOUT$", "w", stderr);

	int result = main(__argc, __argv);

	std::cin.get();

	fclose(inFileStream);
	fclose(outFileStream);
	fclose(errFileStream);

	FreeConsole();

	return result;
}

#endif
