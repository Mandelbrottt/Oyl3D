#pragma once

#include "ModuleInterface.h"

#include "Core/PPExport.h"
#include "Core/Platform/SharedLibrary.h"

namespace Oyl
{
	using ModuleHandle = struct _ModuleHandle*;

	class CORE_EXPORT ModuleManager
	{
	protected:
		ModuleManager();

		~ModuleManager();

	public:
		static
		ModuleManager&
		Get();

		bool
		LoadModule(const String& a_moduleName);

		IModuleInterface*
		GetModuleInterface(const String& a_moduleName) const;

		bool
		UnloadModule(const String& a_moduleName);

		bool
		UnloadModuleUnsafe(const String& a_moduleName);

	private:
		struct Impl;
		Impl* m_impl;
	};
}
