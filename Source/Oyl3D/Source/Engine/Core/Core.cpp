#include "Core/Module/ModuleManager.h"

namespace Oyl
{
	class CoreModule : public IModuleInterface
	{
	public:
		CoreModule()
		{
			puts("New Core!");
		}

		virtual
		~CoreModule()
		{
			puts("Deleting Core!");
		}

		void OnStartModule() override
		{
			puts("OnStart Core!");
		}

		void OnStopModule() override
		{
			puts("OnStop Core!");
		}
	};

	OYL_MODULE_DECLARE(Core, CoreModule)
}
