#include "Resource.h"

namespace Oyl::Internal
{
	ResourceBase::ResourceBase()
		: m_loaded(false) {}

	ResourceBase::~ResourceBase()
	{
		OYL_ASSERT(!m_loaded);
	}

	bool
	ResourceBase::Load()
	{
		m_loaded = true;
		return true;
	}

	bool
	ResourceBase::Unload()
	{
		m_loaded = false;
		return false;
	}

	bool
	ResourceBase::IsLoaded() const noexcept
	{
		return m_loaded;
	}
}
