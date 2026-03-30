#pragma once

#include <string>
#include <vector>

#include "Field.h"
#include "Function.h"
#include "TypeId.h"

namespace Spyll::Reflection
{
	class Assembly;

	class Type final
	{
	public:
		~Type();

		Type(const Type&) = delete;
		Type&
		operator =(const Type&) = delete;

		Type(Type&&) = delete;
		Type&
		operator =(Type&&) = delete;

		TypeId
		GetTypeId() const;

		std::string_view
		GetName() const;

		std::string_view
		GetQualifiedName() const;

		uint32_t
		GetSize() const;

		const std::vector<Type*>&
		GetBaseTypes() const;

		const std::vector<Field*>&
		GetFields() const;

		const std::vector<Function*>&
		GetFunctions() const;

	private:
		Type();

		struct Impl;
		Impl* m_impl;
	};
}
