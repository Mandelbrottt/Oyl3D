#pragma once

#include "ReflectionTest.h"

void Something::_ReflectMembers_(::Oyl::Reflection::Type* a_type)
{
	using This = Something;
	a_type->_Reflect_(
		::Oyl::Reflection::Field {
			"something",
			"something",
			"",
			(::Oyl::uint32) ((size_t) &((This*) 0)->something),
			(::Oyl::uint32) sizeof(This::something),
			::Oyl::GetTypeId<decltype(This::something)>(),
			::Oyl::GetTypeId<This>(),
			::Oyl::Reflection::FieldFlags::None
		}
	);
}

static_assert(true);
