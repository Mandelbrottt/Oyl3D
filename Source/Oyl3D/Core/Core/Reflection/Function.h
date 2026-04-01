#pragma once

#include "Enums.h"
#include "NamedDeclaration.h"
#include "QualifierInfo.h"

namespace Spyll::Reflection
{
	class Type;
	class Assembly;

	class Function : public NamedDeclaration
	{
		friend AssemblyFactory;

	protected:
		Function() = default;

	public:
		const Type*
		GetReturnType() const
		{
			return m_returnType;
		}

		bool
		IsStatic() const
		{
			return m_isStatic;
		}

		bool
		IsReturnConst() const
		{
			return m_isReturnConst;
		}

		bool
		IsReturnVolatile() const
		{
			return m_isReturnVolatile;
		}

		bool
		IsReturnPointer() const
		{
			return m_isReturnPointer || m_isReturnReference;
		}

	private:
		Type* m_returnType = nullptr;

		bool m_isStatic = false;

		bool m_isReturnConst = false;
		bool m_isReturnVolatile = false;
		bool m_isReturnReference = false;
		bool m_isReturnPointer = false;
	};
}
