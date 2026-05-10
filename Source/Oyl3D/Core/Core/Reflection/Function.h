#pragma once

#include "Enums.h"
#include "NamedDeclaration.h"
#include "QualifierInfo.h"
#include "Variable.h"

namespace Oyl::Reflection::Internal {
	struct FunctionParams;
}

namespace Oyl::Reflection
{
	class Type;
	class Assembly;

	namespace Internal
	{
		class ReflectionFactory;
	}

	class Function : public NamedDeclaration
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Function(const Internal::FunctionParams& a_params);

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
		void
		AddParameter(Variable* a_variable);

	private:
		Type* m_returnType = nullptr;

		bool m_isStatic = false;

		bool m_isReturnConst = false;
		bool m_isReturnVolatile = false;
		bool m_isReturnReference = false;
		bool m_isReturnPointer = false;

		std::vector<Variable*> m_parameters;
	};
}
