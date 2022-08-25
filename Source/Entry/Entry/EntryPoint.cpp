#include <Windows.h>
#include <iostream>
#include <vector>

#include <Core/Application/Main.h>

static void SetupConsole();

static void ShutdownConsole();

// ReSharper disable CppInconsistentNaming
int WINAPI wWinMain(
	_In_ HINSTANCE     hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR        lpCmdLine,
	_In_ int           nShowCmd
)
// ReSharper restore CppInconsistentNaming
{
	(void) hInstance;
	(void) hPrevInstance;
	(void) lpCmdLine;
	(void) nShowCmd;

	SetupConsole();
	
	Rearm::Init();

	//do 
	//{
	//	extern void LoadFromDll();
	//	LoadFromDll();

	//	printf("press q to stop\n");
	//} while (getchar() != 'q');

	std::cin.get();
	
	Rearm::Shutdown();

	ShutdownConsole();

	return 0;
}

void SetupConsole()
{
	// Attach to the console if launched from the command line, otherwise create one
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
		AllocConsole();

	FILE* inFileStream;
	FILE* outFileStream;
	FILE* errFileStream;
	
	// Direct input and output to the console
	freopen_s(&inFileStream,  "CONIN$",  "r", stdin);
	freopen_s(&outFileStream, "CONOUT$", "w", stdout);
	freopen_s(&errFileStream, "CONOUT$", "w", stderr);
}

void ShutdownConsole()
{	
	FreeConsole();
}
