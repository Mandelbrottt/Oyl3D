#include "ReflectionFactory.h"

#include "Assembly.h"

using Type = Oyl::Reflection::Type;
using Field = Oyl::Reflection::Field;
using Function = Oyl::Reflection::Function;

namespace Oyl::Reflection::Internal
{
	Oyl::Reflection::Assembly*
	ReflectionFactory::CreateAssembly(const AssemblyParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(Assembly), (std::align_val_t) alignof(Assembly));
		Assembly* assembly = std::launder(reinterpret_cast<Assembly*>(buf));
		new(assembly) Assembly(a_params);

		return assembly;
	}

	Type*
	ReflectionFactory::AddTypeToAssembly(Assembly* a_assembly, const TypeParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(Type), (std::align_val_t) alignof(Type));
		Type* type = std::launder(reinterpret_cast<Type*>(buf));
		new(type) Type(a_params);

		a_assembly->AddType(type);
		return type;
	}

	Field*
	ReflectionFactory::AddFieldToType(Type* a_type, const FieldParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(Field), (std::align_val_t) alignof(Field));
		Field* field = std::launder(reinterpret_cast<Field*>(buf));
		new(field) Field(a_params);

		a_type->AddField(field);
		return field;
	}

	MemberFunction*
	ReflectionFactory::AddFunctionToType(Type* a_type, const MemberFunctionParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(MemberFunction), (std::align_val_t) alignof(MemberFunction));
		MemberFunction* function = std::launder(reinterpret_cast<MemberFunction*>(buf));
		new(function) MemberFunction(a_params);

		a_type->AddFunction(function);
		return function;
	}

	Variable*
	ReflectionFactory::AddParameterToFunction(Function* a_function, const VariableParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(Variable), (std::align_val_t) alignof(Variable));
		Variable* variable = std::launder(reinterpret_cast<Variable*>(buf));
		new(variable) Variable(a_params);

		a_function->AddParameter(variable);
		return variable;
	}
}
