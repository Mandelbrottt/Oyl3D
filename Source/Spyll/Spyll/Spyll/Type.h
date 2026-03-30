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
		friend
		void
		::_PopulateReflectionAssembly(Assembly*);
		
		Type();

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

		bool
		IsPrimitive() const;

		const std::vector<Type*>&
		GetBaseTypes() const;

		const std::vector<Field*>&
		GetFields() const;

		const std::vector<Function*>&
		GetFunctions() const;

		template<typename T>
		static
		const Type*
		Get();

		static
		const Type*
		Get(std::string_view a_name);

	private:
		TypeId m_typeId = TypeId::Null;

		uint32_t m_size = 0;
		
		std::string m_name;
		std::string m_qualifiedName;

		bool m_isPrimitive = false;

		Assembly* m_assembly = nullptr;

		std::vector<Type*> m_baseTypes;

		std::vector<Function*> m_functions;
		std::vector<Field*> m_fields;
	};

	template<typename T>
	const Type*
	Type::Get()
	{
		return Get(typeid(T).name());
	}
}
