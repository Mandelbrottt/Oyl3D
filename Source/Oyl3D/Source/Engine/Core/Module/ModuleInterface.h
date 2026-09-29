#pragma once

namespace Oyl
{
	class IModuleInterface
	{
	public:
		virtual
		~IModuleInterface() = default;

		virtual
		void
		OnStartModule() {}

		virtual
		void
		OnStopModule() {}

		virtual
		bool
		SupportsHotReload()
		{
			return true;
		}
	};
}
