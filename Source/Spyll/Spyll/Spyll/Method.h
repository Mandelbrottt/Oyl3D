#pragma once

#include "Function.h"
#include "MemberInfo.h"

namespace Spyll::Reflection
{
	class Method : public Function, public MemberInfo
	{
		friend AssemblyFactory;

	protected:
		Method() = default;

	public:
		bool
		IsConstructor() const
		{
			return m_constructorType != ConstructorType::None;
		}

		ConstructorType
		GetConstructorType() const
		{
			return m_constructorType;
		}

		bool
		IsVirtual() const
		{
			return m_isVirtual;
		}

		bool
		IsPureVirtual() const
		{
			return m_isPureVirtual;
		}

		bool
		IsOverride() const
		{
			return m_isOverride;
		}

		bool
		IsConst() const
		{
			return m_isConst;
		}

		bool
		IsVolatile() const
		{
			return m_isVolatile;
		}

	private:
		ConstructorType m_constructorType = ConstructorType::None;

		bool m_isVirtual = false;
		bool m_isPureVirtual = false;
		bool m_isOverride = false;

		bool m_isConst = false;
		bool m_isVolatile = false;
	};
}