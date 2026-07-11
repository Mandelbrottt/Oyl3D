#include "Resource.h"

namespace Oyl
{
	Resource::Resource()
		: m_loaded(false),
		  m_dirty(true) {}

	Resource::~Resource()
	{
		OYL_ASSERT(!m_loaded);
	}

	bool
	Resource::Load()
	{
		m_loaded = true;
		return true;
	}

	bool
	Resource::Unload()
	{
		m_loaded = false;
		return false;
	}

	bool
	Resource::IsLoaded() const noexcept
	{
		return m_loaded;
	}

	bool
	Resource::IsDirty() const noexcept
	{
		return m_dirty;
	}
}
