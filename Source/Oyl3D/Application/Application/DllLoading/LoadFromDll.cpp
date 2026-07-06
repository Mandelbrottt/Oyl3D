//#include <Windows.h>
//#include <Core/Testing/Interface.h>
//
//void LoadFromDll()
//{
//	HMODULE dllTest = LoadLibrary(TEXT("DllTest.dll"));
//
//	if (dllTest)
//	{
//		using GetInterfacesFn = const std::vector<std::string>*(*)();
//		using InterfaceSizeFn = size_t(*)();
//		using AllocInterfaceFn = Interface*(*)(void*, size_t);
//
//		GetInterfacesFn getInterfacesFn = (GetInterfacesFn) GetProcAddress(dllTest, "__DllTest_Interfaces");
//
//		if (getInterfacesFn == nullptr)
//		{
//			return;
//		}
//
//		const std::vector<std::string> interfaces = *getInterfacesFn();
//
//		for (auto const& interfaceName : interfaces)
//		{
//			const std::string interfaceSizeFnName = "__Size" + interfaceName;
//			InterfaceSizeFn   interfaceSizeFn = (InterfaceSizeFn) GetProcAddress(dllTest, interfaceSizeFnName.c_str());
//
//			const std::string allocInterfaceFnName = "__Alloc" + interfaceName;
//			AllocInterfaceFn  allocInterfaceFn     = (AllocInterfaceFn) GetProcAddress(
//				dllTest,
//				allocInterfaceFnName.c_str()
//			);
//
//			if (interfaceSizeFn == nullptr || allocInterfaceFn == nullptr)
//			{
//				return;
//			}
//
//			size_t interfaceSize     = interfaceSizeFn();
//			void*  interfaceLocation = malloc(interfaceSize);
//
//			Interface* interfaceObj = allocInterfaceFn(interfaceLocation, interfaceSize);
//
//			interfaceObj->Foo();
//
//			interfaceObj->~Interface();
//
//			free(interfaceLocation);
//			interfaceLocation = interfaceObj = nullptr;
//		}
//
//		FreeLibrary(dllTest);
//	}
//}
