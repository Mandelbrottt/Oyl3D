#pragma once

namespace Oyl::Reflection
{
	namespace Internal
	{
		struct QualifierInfoParams;
		class ReflectionFactory;
	}

	class QualifierInfo
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		QualifierInfo(const Internal::QualifierInfoParams& a_params);

	public:
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
		bool m_isConst = false;
		bool m_isVolatile = false;
	};
}
