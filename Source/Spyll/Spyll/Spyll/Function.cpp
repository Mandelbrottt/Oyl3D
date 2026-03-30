#include "Function.h"

namespace Spyll::Reflection
{
	Function::Function() {}

	Function::~Function() {}

	std::string_view
	Function::GetName() const
	{
		return m_name;
	}

	std::string_view
	Function::GetQualifiedName() const
	{
		return m_qualifiedName;
	}

	const Type*
	Function::GetReturnType() const
	{
		return m_returnType;
	}

	bool
	Function::IsMember() const
	{
		return m_ownerType != nullptr;
	}

	const Type*
	Function::GetOwningType() const
	{
		return m_ownerType;
	}

	bool
	Function::IsConstructor() const
	{
		return m_constructorType != ConstructorType::None;
	}

	ConstructorType
	Function::GetConstructorType() const
	{
		return m_constructorType;
	}

	bool
	Function::IsVirtual() const
	{
		return m_isVirtual;
	}

	bool
	Function::IsPureVirtual() const
	{
		return m_isPureVirtual;
	}

	bool
	Function::IsOverride() const
	{
		return m_isOverride;
	}

	bool
	Function::IsStatic() const
	{
		return m_isStatic;
	}

	bool
	Function::IsConst() const
	{
		return m_isConst;
	}

	bool
	Function::IsVolatile() const
	{
		return m_isVolatile;
	}

	bool
	Function::IsReturnConst() const
	{
		return m_isReturnConst;
	}

	bool
	Function::IsReturnVolatile() const
	{
		return m_isReturnVolatile;
	}

	bool
	Function::IsReturnPointer() const
	{
		return m_isReturnPointer || m_isReturnReference;
	}
}
