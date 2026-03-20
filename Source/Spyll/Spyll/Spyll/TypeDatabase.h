#pragma once

namespace Spyll::Reflection
{
	class TypeDatabase final
	{
	public:
		TypeDatabase();

		~TypeDatabase();
	private:
		struct Impl;
		Impl* m_impl;
	};
}