#pragma once

#include "Enum.h"
#include "ReflectionParams.h"
#include "Type.h"

namespace Oyl::Reflection::Internal
{
	using ReflectionAllocatorFn = void*(*)(std::size_t, std::align_val_t);

	class OYL_CORE_API ReflectionFactory
	{
		ReflectionFactory() = delete;

	public:
		static
		Assembly*
		CreateAssembly(const AssemblyParams& a_params, ReflectionAllocatorFn a_allocate);

		template<typename TAttribute>
		static
		const Attribute*
		AddAttributeToDeclaration(NamedDeclaration* a_declaration, TAttribute* a_attr)
		{
			a_attr->m_type = Type::Get<TAttribute>();
			auto& result = a_declaration->m_attributes.emplace_back(a_attr);
			return result;
		}

		static
		Type*
		AddTypeToAssembly(Assembly* a_assembly, const TypeParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Variable*
		AddVariableToType(Type* a_type, const VariableParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Field*
		AddFieldToType(Type* a_type, const FieldParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Function*
		AddFunctionToType(Type* a_type, const FunctionParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Method*
		AddMethodToType(Type* a_type, const MethodParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Argument*
		AddArgumentToInvokable(Invokable* a_function, const ArgumentParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Variable*
		AddGlobalVariableToAssembly(Assembly* a_assembly, const VariableParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Function*
		AddGlobalFunctionToAssembly(Assembly* a_assembly, const FunctionParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		Enum*
		AddEnumToAssembly(Assembly* a_assembly, const EnumParams& a_params, ReflectionAllocatorFn a_allocate);

		static
		EnumValue*
		AddValueToEnum(Enum* a_enum, const EnumValueParams& a_params, ReflectionAllocatorFn a_allocate);
	};

	// This is a rewrite of the technique showed here:
	// http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html
	// https://gist.github.com/dabrahams/1528856

	/// Generate a static member of type \c Tag::type in which to store the address of the private member.
	/// It is crucial that Tag does not depend on the \b value of the stored address in any way so that
	/// we can access it from ordinary code without directly touching private data.
	///
	/// \tparam Tag A unique type used as a key to set and retrieve a given private member's member pointer
	/// Tag types should be of the form
	/// \code{.cpp}
	/// struct Tag { using type = <type of address being stolen>; };
	/// \endcode
	template<typename Tag>
	struct StolenMember
	{
		static typename Tag::type value;
	};

	template<typename Tag>
	typename Tag::type StolenMember<Tag>::value;

	template<typename Tag>
	inline const typename Tag::type& StolenMember_V = StolenMember<Tag>::value;

	/// Generate a static member "instance" whose constructor initializes StolenMember<Tag>::value.
	/// This type will only be named in an explicit template instantiation, where it is legal to
	/// pass the address of a private member
	///
	/// \tparam Tag A unique type used as a key to set and retrieve a given private member's member pointer
	/// Tag types should be of the form
	/// \code{.cpp}
	/// struct Tag { using type = <type of address being stolen>; };
	/// \endcode
	///
	///	\tparam X The address of the member to steal
	template<class Tag, typename Tag::type X>
	struct StealMember
	{
		StealMember() { StolenMember<Tag>::value = X; }

		static StealMember instance;
	};

	template<class Tag, typename Tag::type X>
	StealMember<Tag, X> StealMember<Tag, X>::instance;

	/*
	 *	Example Usage:
	 *
	 *	class Private
	 *	{
	 *	private:
	 *		int member_field;
	 *
	 *		static unsigned static_field;
	 *
	 *		void MemberFunction(float);
	 *
	 *		static double StaticFunction();
	 *	};
	 *
	 *	struct _Steal_Private_member_field { using type = int Private::*; };
	 *	template struct StealMember<_Steal_Private_member_field, &Private::member_field>;
	 *
	 *	struct _Steal_Private_static_field { using type = unsigned*; };
	 *	template struct StealMember<_Steal_Private_static_field, &Private::static_field>;
	 *
	 *	struct _Steal_Private_MemberFunction { using type = void(Private::*)(float); };
	 *	template struct StealMember<_Steal_Private_MemberFunction, &Private::MemberFunction>;
	 *
	 *	struct _Steal_Private_StaticFunction { using type = double(*)(); };
	 *	template struct StealMember<_Steal_Private_StaticFunction, &Private::StaticFunction>;
	 *
	 *	void Foo()
	 *	{
	 *		Private p;
	 *
	 *		// Equivalent to p.member_field = 5
	 *		p.*(StolenMember<_Steal_Private_member_field>::value) = 5;
	 *
	 *		// Equivalent to Private::static_field = 6u
	 *		*StolenMember_V<_Steal_Private_static_field> = 6u;
	 *
	 *		// Equivalent to p.MemberFunction(7.0f)
	 *		(p.*StolenMember_V<_Steal_Private_MemberFunction>)(7.0f);
	 *
	 *		// Equivalent to double value = Private::StaticFunction();
	 *		double value = (StolenMember<_Steal_Private_member_field>::value)();
	 *	}
	 *
	 */

	// Type smuggling https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/
}
