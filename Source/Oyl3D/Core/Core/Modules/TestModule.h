#pragma once

#include "../.Generated/Headers/TestModule.generated.h"

#include "Core/Common.h"
#include "Core/Application/Module.h"

namespace Oyl
{
	class OYL_CORE_API TestModule : public Module
	{
		GeneratedBody();

	public:
		void
		OnInit() override;

		void
		OnUpdate() override;

		void
		OnShutdown() override;
	};
}
