#include "ReflectionFactory.h"

using Type = Oyl::Reflection::Type;
using Field = Oyl::Reflection::Field;
using Function = Oyl::Reflection::Function;

namespace Oyl::Reflection::Internal
{
	Type*
	ReflectionFactory::CreateType(const TypeParams& a_params)
	{
		Type* type = new Type(a_params);
		return type;
	}

	Field*
	ReflectionFactory::AddFieldToType(Type* a_type, const FieldParams& a_params)
	{
		Field* field = new Field(a_params);
		a_type->AddField(field);
		return field;
	}

	MemberFunction*
	ReflectionFactory::AddFunctionToType(Type* a_type, const MemberFunctionParams& a_params)
	{
		MemberFunction* function = new MemberFunction(a_params);
		a_type->AddFunction(function);
		return function;
	}
}
