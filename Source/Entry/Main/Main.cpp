#include <Windows.h>
#include <iostream>

#if REFLY_EDITOR
	#include "Editor/main.h"
#endif

#include <Core/Interface.h>

#include "Core/main.h"

static void SetupConsole();

static void ShutdownConsole();

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    pCmdLine,
	_In_ int       nCmdShow
)
{
	(void) hInstance;
	(void) hPrevInstance;
	(void) pCmdLine;
	(void) nCmdShow;

	SetupConsole();
	
	std::cout << "Hello World!\n";

#if REFLY_EDITOR
	Something();
#endif
	
	SomethingElse();

	do 
	{
		HMODULE dllTest = LoadLibrary(TEXT("DllTest.dll"));

		if (dllTest)
		{
			size_t (*interfaceSizeFn)() = (size_t(*)()) GetProcAddress(dllTest, "InterfaceSize");
			Interface* (*getInterfaceFn)(void*, size_t) = (Interface* (*)(void*, size_t)) GetProcAddress(dllTest, "AllocInterface");

			if (interfaceSizeFn == nullptr || getInterfaceFn == nullptr)
			{
				goto fail_to_grab_procs;
			}
			
			size_t interfaceSize = interfaceSizeFn();
			void* interfaceLocation = malloc(interfaceSize);

			Interface* interfaceObj = getInterfaceFn(interfaceLocation, interfaceSize);

			interfaceObj->Foo();

			interfaceObj->~Interface();

			free(interfaceLocation);
			interfaceLocation = interfaceObj = nullptr;

			FreeLibrary(dllTest);
		}

	fail_to_grab_procs:
		printf("press q to stop\n");
	} while (getchar() != 'q');

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
