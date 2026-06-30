#include "ReflectionFactory.h"

#include <Spyll/Tool/Core/Declarations/Enum.h>

#include "Assembly.h"
#include "Enum.h"

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

	const Attribute*
	ReflectionFactory::AddAttributeToDeclaration(NamedDeclaration* a_declaration, const Attribute* a_attr)
	{
		auto& result = a_declaration->m_attributes.emplace_back(a_attr);
		return result;
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

	Variable*
	ReflectionFactory::AddVariableToType(Type* a_type, const VariableParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(Variable), (std::align_val_t) alignof(Variable));
		Variable* field = std::launder(reinterpret_cast<Variable*>(buf));
		new(field) Variable(a_params);

		a_type->AddVariable(field);
		return field;
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

	Function*
	ReflectionFactory::AddFunctionToType(Type* a_type, const FunctionParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(Function), (std::align_val_t) alignof(Function));
		Function* function = std::launder(reinterpret_cast<Function*>(buf));
		new(function) Function(a_params);

		a_type->AddFunction(function);
		return function;
	}

	Method*
	ReflectionFactory::AddMethodToType(Type* a_type, const MethodParams& a_params, ReflectionAllocatorFn a_allocate)
	{
		void* buf = a_allocate(sizeof(Method), (std::align_val_t) alignof(Method));
		Method* method = std::launder(reinterpret_cast<Method*>(buf));
		new(method) Method(a_params);

		a_type->AddMethod(method);
		return method;
	}

	Argument*
	ReflectionFactory::AddArgumentToInvokable(
		Invokable* a_function,
		const ArgumentParams& a_params,
		ReflectionAllocatorFn a_allocate
	)
	{
		(void) a_allocate;

		auto* argument = a_function->AddArgument(a_params);
		return argument;
	}

	Variable*
	ReflectionFactory::AddGlobalVariableToAssembly(
		Assembly* a_assembly,
		const VariableParams& a_params,
		ReflectionAllocatorFn a_allocate
	)
	{
		void* buf = a_allocate(sizeof(Variable), (std::align_val_t) alignof(Variable));
		Variable* variable = std::launder(reinterpret_cast<Variable*>(buf));
		new(variable) Variable(a_params);

		a_assembly->AddGlobalVariable(variable);
		return variable;
	}

	Function*
	ReflectionFactory::AddGlobalFunctionToAssembly(
		Assembly* a_assembly,
		const FunctionParams& a_params,
		ReflectionAllocatorFn a_allocate
	)
	{
		void* buf = a_allocate(sizeof(Function), (std::align_val_t) alignof(Function));
		Function* function = std::launder(reinterpret_cast<Function*>(buf));
		new(function) Function(a_params);

		a_assembly->AddGlobalFunction(function);
		return function;
	}

	Enum*
	ReflectionFactory::AddEnumToAssembly(
		Assembly* a_assembly,
		const EnumParams& a_params,
		ReflectionAllocatorFn a_allocate
	)
	{
		void* buf = a_allocate(sizeof(Enum), (std::align_val_t) alignof(Enum));
		Enum* enum_ = std::launder(reinterpret_cast<Enum*>(buf));
		new(enum_) Enum(a_params);

		a_assembly->AddEnum(enum_);
		return enum_;
	}

	EnumValue*
	ReflectionFactory::AddValueToEnum(
		Enum* a_enum,
		const EnumValueParams& a_params,
		ReflectionAllocatorFn a_allocate
	)
	{
		(void) a_allocate;

		auto value = a_enum->AddValue(a_params);
		return value;
	}
}
