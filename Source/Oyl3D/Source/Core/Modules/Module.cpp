#include "pch.h"
#include "Module.h"

namespace Oyl
{
	Module::Module() {}

	Module::~Module() {}

	Module*
	Module::GetModule(Reflection::TypeId a_typeId)
	{
		auto* moduleRegistry = ModuleRegistry::GetCurrent();
		return moduleRegistry->GetModule(a_typeId);
	}
}
