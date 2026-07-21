#include "Resource.h"

namespace Oyl::Internal
{
	ResourceBase::ResourceBase() {}

	ResourceBase::~ResourceBase()
	{
		OYL_ASSERT(!m_loaded);
	}

	bool
	ResourceBase::Load()
	{
		m_loaded = true;
		m_dirty = false;
		return true;
	}

	bool
	ResourceBase::Unload()
	{
		m_loaded = false;
		m_dirty = true;
		return false;
	}

	bool
	ResourceBase::IsLoaded() const
	{
		return m_loaded;
	}

	bool
	ResourceBase::IsDirty() const
	{
		return m_dirty;
	}

	void
	ResourceBase::SetDirty()
	{
		m_dirty = true;
	}
}
