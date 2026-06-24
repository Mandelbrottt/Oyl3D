#pragma once

#include "Invokable.h"
#include "NamedDeclaration.h"
#include "ReflectionParams.h"

namespace Oyl::Reflection
{
	class Type;
	class Assembly;

	namespace Internal
	{
		class ReflectionFactory;
	}

	class Function : public NamedDeclaration, public Invokable
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Function(const Internal::FunctionParams& a_params);

	public:
		virtual
		~Function();
	};
}
