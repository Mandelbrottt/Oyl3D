#pragma once

#include "ReflectionParams.h"

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class ReflectionFactory;
	}

	struct Argument
	{
		Type* type = nullptr;
		std::string name;
	};

	struct Signature
	{
		Type* returnType = nullptr;
		std::vector<Argument> arguments;
	};

	class Invokable
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Invokable(const Internal::InvokableParams& a_params);

	public:
		virtual
		~Invokable();

	protected:
		Signature m_signature;

	private:
		Argument*
		AddArgument(const Internal::ArgumentParams& a_argument);
	};
}