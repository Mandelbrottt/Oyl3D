#include <Windows.h>
#include <iostream>
#include <vector>

#include <Core/Interface.h>

#include <Core/Application/Main.h>

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
	
	Refly::Init();
	
	do 
	{
		HMODULE dllTest = LoadLibrary(TEXT("DllTest.dll"));

		if (dllTest)
		{
			using GetInterfacesFn  = const std::vector<std::string>&(*)();
			using InterfaceSizeFn  = size_t(*)();
			using AllocInterfaceFn = Interface*(*)(void*, size_t);
			
			GetInterfacesFn  getInterfacesFn   = (GetInterfacesFn)  GetProcAddress(dllTest, "__DllTest_Interfaces");

			if (getInterfacesFn == nullptr)
			{
				goto fail_to_grab_procs;
			}

			const std::vector<std::string>& interfaces = getInterfacesFn();

			for (auto const& interfaceName : interfaces)
			{
				const std::string interfaceSizeFnName = "__Size" + interfaceName;
				InterfaceSizeFn  interfaceSizeFn = (InterfaceSizeFn)  GetProcAddress(dllTest, interfaceSizeFnName.c_str());

				const std::string allocInterfaceFnName = "__Alloc" + interfaceName;
				AllocInterfaceFn allocInterfaceFn  = (AllocInterfaceFn) GetProcAddress(dllTest, allocInterfaceFnName.c_str());

				if (interfaceSizeFn == nullptr || allocInterfaceFn == nullptr)
				{
					goto fail_to_grab_procs;
				}

				size_t interfaceSize = interfaceSizeFn();
				void* interfaceLocation = malloc(interfaceSize);

				Interface* interfaceObj = allocInterfaceFn(interfaceLocation, interfaceSize);

				interfaceObj->Foo();

				interfaceObj->~Interface();

				free(interfaceLocation);
				interfaceLocation = interfaceObj = nullptr;
			}
			
			FreeLibrary(dllTest);
		}

	fail_to_grab_procs:
		printf("press q to stop\n");
	} while (getchar() != 'q');
	
	Refly::Shutdown();

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
