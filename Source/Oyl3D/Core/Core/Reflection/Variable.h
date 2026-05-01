#pragma once

#include "Enums.h"
#include "NamedDeclaration.h"
#include "QualifierInfo.h"

namespace Oyl::Reflection
{
	class Type;
	
	namespace Internal
	{
		class AssemblyFactory;
	}

	class Variable : public NamedDeclaration, public QualifierInfo
	{
		friend Internal::AssemblyFactory;

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
