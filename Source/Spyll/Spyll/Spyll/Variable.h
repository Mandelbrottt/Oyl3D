#pragma once

#include "Enums.h"
#include "NamedDeclaration.h"
#include "QualifierInfo.h"

namespace Spyll::Reflection
{
	class Type;

	class Variable : public NamedDeclaration, public QualifierInfo
	{
	protected:
		Variable() = default;

	public:
		const Type*
		GetType() const
		{
			return m_type;
		}
		
		bool
		IsPointer() const
		{
			return m_isPointer;
		}

		bool
		IsReference() const
		{
			return m_isReference;
		}

	private:
		Type* m_type = nullptr;

		bool m_isReference = false;
		bool m_isPointer = false;
	};
}
