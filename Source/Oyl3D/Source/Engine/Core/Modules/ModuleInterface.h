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

	class ReloadableModule : public IModuleInterface
	{
	public:
		bool
		SupportsHotReload() override
		{
			return true;
		}
	};

	class NonReloadableModule : public IModuleInterface
	{
	public:
		bool
		SupportsHotReload() override
		{
			return false;
		}
	};
}
