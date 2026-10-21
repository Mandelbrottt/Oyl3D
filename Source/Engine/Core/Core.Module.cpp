#include "Core/Modules/ModuleManager.h"

namespace Oyl
{
	class CoreModule : public IModuleInterface
	{
	public:
		bool
		SupportsHotReload() override { return false; }

		bool
		ShouldAutoload() override { return true; }
	};

	OYL_MODULE_DECLARE(Core, CoreModule)
}
