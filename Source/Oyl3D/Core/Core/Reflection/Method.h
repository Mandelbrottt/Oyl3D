#pragma once

#include "Function.h"
#include "ReflectionParams.h"

namespace Oyl::Reflection
{
	namespace Internal
	{
		class ReflectionFactory;
	}

	class Method : public Function
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Method(const Internal::MethodParams& a_params);

	public:
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

	private:
		bool m_isVirtual = false;
		bool m_isPureVirtual = false;
	};
}