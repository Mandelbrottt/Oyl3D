#pragma once

#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	struct Event
	{
		virtual
		~Event() = default;

		virtual
		Reflection::TypeId
		GetTypeId() const = 0;
	};
}

#define OYL_DECLARE_EVENT() \
	constexpr ::Oyl::Reflection::TypeId \
	GetTypeId() const override \
	{ \
		return ::Oyl::Reflection::GetTypeId<decltype(*this)>();\
	} \
	OYL_FORCE_SEMICOLON
