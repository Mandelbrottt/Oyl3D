#include "Resource.h"

namespace Oyl
{
	ResourceBase::ResourceBase()
		: m_loaded(false),
		  m_dirty(true) {}

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

	bool
	ResourceBase::IsDirty() const noexcept
	{
		return m_dirty;
	}

	bool
	ResourceBase::Compile(void* /*a_customData*/)
	{
		m_dirty = false;
		return true;
	}

	void
	ResourceBase::SetDirty() noexcept
	{
		m_dirty = true;
	}
}
