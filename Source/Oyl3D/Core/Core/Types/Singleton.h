#pragma once

#include "Core/Common.h"

namespace Oyl
{
	template<typename TClass>
	class OYL_CORE_API Singleton
	{
	protected:
		Singleton() = default;

	public:
		Singleton(Singleton&) = delete;
		Singleton&
		operator =(Singleton&) = delete;
		
		static
		TClass& 
		Instance()
		{
			static TClass instance;
			return instance;
		}
	};
}
