#pragma once

#include "Core/Delegate.h"
#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	struct Event
	{
		virtual
		~Event() = default;

		const Reflection::TypeId typeId;

	protected:
		explicit
		constexpr
		Event(Reflection::TypeId a_typeId)
			: typeId(a_typeId) {}
	};

	template<typename TEvent>
	struct EventBase : Event
	{
	protected:
		constexpr
		EventBase()
			: Event(Reflection::GetTypeId<TEvent>()) {}
	};

	using EventDelegate = Delegate<void(const Event&)>;
}