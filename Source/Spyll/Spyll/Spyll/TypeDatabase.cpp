#include "TypeDatabase.h"

namespace Spyll::Reflection
{
	struct TypeDatabase::Impl
	{
		
	};
	
	TypeDatabase::TypeDatabase()
	{
		m_impl = new Impl;
	}

	TypeDatabase::~TypeDatabase()
	{
		delete m_impl;
		m_impl = nullptr;
	}
}
