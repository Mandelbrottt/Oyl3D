#include "Function.h"

namespace Spyll::Reflection
{
	struct Function::Impl
	{
		std::string name;
		std::string qualifiedName;

		Type* returnType = nullptr;

		Type* ownerType = nullptr;

		AccessSpecifier accessSpecifier = AccessSpecifier::None;
		ConstructorType constructorType = ConstructorType::None;

		bool isVirtual = false;
		bool isPureVirtual = false;
		bool isOverride = false;

		bool isStatic = false;
		bool isConst = false;
		bool isVolatile = false;

		bool isReturnConst = false;
		bool isReturnVolatile = false;
		bool isReturnReference = false;
		bool isReturnPointer = false;
	};

	Function::Function()
	{
		m_impl = new Impl;
	}

	Function::~Function()
	{
		delete m_impl;
		m_impl = nullptr;
	}

	std::string_view
	Function::GetName() const
	{
		return m_impl->name;
	}

	std::string_view
	Function::GetQualifiedName() const
	{
		return m_impl->qualifiedName;
	}

	const Type*
	Function::GetReturnType() const
	{
		return m_impl->returnType;
	}

	bool
	Function::IsMember() const
	{
		return m_impl->ownerType != nullptr;
	}

	const Type*
	Function::GetOwningType() const
	{
		return m_impl->ownerType;
	}

	bool
	Function::IsConstructor() const
	{
		return m_impl->constructorType != ConstructorType::None;
	}

	ConstructorType
	Function::GetConstructorType() const
	{
		return m_impl->constructorType;
	}

	bool
	Function::IsVirtual() const
	{
		return m_impl->isVirtual;
	}

	bool
	Function::IsPureVirtual() const
	{
		return m_impl->isPureVirtual;
	}

	bool
	Function::IsOverride() const
	{
		return m_impl->isOverride;
	}

	bool
	Function::IsStatic() const
	{
		return m_impl->isStatic;
	}

	bool
	Function::IsConst() const
	{
		return m_impl->isConst;
	}

	bool
	Function::IsVolatile() const
	{
		return m_impl->isVolatile;
	}

	bool
	Function::IsReturnConst() const
	{
		return m_impl->isReturnConst;
	}

	bool
	Function::IsReturnVolatile() const
	{
		return m_impl->isReturnVolatile;
	}

	bool
	Function::IsReturnPointer() const
	{
		return m_impl->isReturnPointer || m_impl->isReturnReference;
	}
}
