#pragma once

#include "Enum.h"
#include "ReflectionParams.h"
#include "Type.h"

namespace Oyl::Reflection::Internal
{
	using ReflectionAllocatorFn = void*(*)(std::size_t, std::align_val_t);

	class ReflectionFactory
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

	// Generate a static data member of type Stub::type in which to store
	// the address of a private member.  It is crucial that Stub does not
	// depend on the /value/ of the the stored address in any way so that
	// we can access it from ordinary code without directly touching
	// private data.
	template<typename Stub>
	struct Stowed
	{
		static typename Stub::type value;
	};

	template<typename Stub>
	typename Stub::type Stowed<Stub>::value;

	// Generate a static data member whose constructor initializes
	// Stowed<Stub>::value.  This type will only be named in an explicit
	// instantiation, where it is legal to pass the address of a private
	// member.
	template<class Stub, typename Stub::type X>
	struct StowPrivate
	{
		StowPrivate() { Stowed<Stub>::value = X; }

		static StowPrivate instance;
	};

	template<class Stub, typename Stub::type X>
	StowPrivate<Stub, X> StowPrivate<Stub, X>::instance;
}
