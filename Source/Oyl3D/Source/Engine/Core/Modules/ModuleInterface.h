#pragma once

namespace Oyl
{
	class IModuleInterface
	{
	public:
		virtual
		~IModuleInterface() = 0;

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
			return false;
		}

		virtual
		bool
		ShouldAutoload()
		{
			return true;
		}
	};

	inline
	IModuleInterface::~IModuleInterface() {}

	class ReloadableModule : public IModuleInterface
	{
	public:
		virtual
		~ReloadableModule() = default;

		bool
		SupportsHotReload() override
		{
			return true;
		}
	};

	class NonReloadableModule : public IModuleInterface
	{
	public:
		virtual
		~NonReloadableModule() = default;

		bool
		SupportsHotReload() override
		{
			return false;
		}
	};
}
