#pragma once

#include "Type.h"

namespace Oyl::Reflection
{
	template<class T>
	Type& Type::Get() noexcept
	{
		Oyl::TypeId id = GetTypeId<T>();

		auto iter = Types().find(id);
		if (iter == Types().end())
		{
			iter = Register<T>();
		}
		return iter->second;
	}
}
