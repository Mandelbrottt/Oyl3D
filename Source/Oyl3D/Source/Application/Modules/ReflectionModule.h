#pragma once

#include <Core/Modules/Module.h>

namespace Oyl
{
	class ReflectionModule : public Module
	{
		OYL_DECLARE_MODULE(ReflectionModule);

	public:
		void
		OnCreate() override;

		void
		OnInit() override;

		void
		OnUpdate() override;

		void
		OnDestroy() override;
	};
}
