#pragma once

#include <vector>

#include "Field.h"
#include "Function.h"
#include "Method.h"
#include "ReflectionParams.h"
#include "TypeId.h"

namespace Oyl::Reflection
{
	class Assembly;
	
	namespace Internal
	{
		class ReflectionFactory;
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

		const std::vector<Variable*>&
		GetVariables() const;

		const std::vector<Field*>&
		GetFields() const;

		const std::vector<Function*>&
		GetFunctions() const;

		const std::vector<Method*>&
		GetMethods() const;

	private:
		void
		AddBaseType(Type* a_type);

		void
		AddVariable(Variable* a_variable);

		void
		AddField(Field* a_field);

		void
		AddFunction(Function* a_function);

		void
		AddMethod(Method* a_method);

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
