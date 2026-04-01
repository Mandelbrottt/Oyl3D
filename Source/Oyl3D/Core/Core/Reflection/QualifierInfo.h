#pragma once

namespace Spyll
{
	class AssemblyFactory;
}

namespace Spyll::Reflection
{
	class QualifierInfo
	{
		friend AssemblyFactory;

	protected:
		QualifierInfo() = default;

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