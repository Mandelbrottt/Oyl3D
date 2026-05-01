#pragma once

#include <string>
#include <vector>

#include "Field.h"
#include "Function.h"
#include "MemberFunction.h"
#include "TypeId.h"

namespace Oyl::Reflection
{
	class Assembly;
	
	namespace Internal
	{
		class ReflectionFactory;
		struct TypeParams;
	}

	class Type final
	{
		friend Internal::ReflectionFactory;
		
		explicit
		Type(const Internal::TypeParams& a_params);

	public:
		~Type();

		Type(const Type&) = delete;
		Type&
		operator =(const Type&) = delete;

		Type(Type&&) = delete;
		Type&
		operator =(Type&&) = delete;
		
		template<typename T>
		static
		Type*
		Get();

		static
		Type*
		Get(std::string_view a_name);

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

		const std::vector<MemberFunction*>&
		GetFunctions() const;

	private:
		void
		AddBaseType(Type* a_type);

		void
		AddField(Field* a_field);

		void
		AddFunction(MemberFunction* a_method);

	private:
		struct Impl;
		Impl* m_impl;
	};

	template<typename T>
	Type*
	Type::Get()
	{
		return Get(typeid(T).name());
	}
}
