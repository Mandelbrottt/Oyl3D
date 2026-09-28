#pragma once

namespace Oyl
{
	class IModuleInterface
	{
	protected:
		virtual
		~IModuleInterface() = default;

	public:
		virtual
		void
		OnPreInit() {}

		virtual
		void
		OnPostInit() {}

	public:
	};
}