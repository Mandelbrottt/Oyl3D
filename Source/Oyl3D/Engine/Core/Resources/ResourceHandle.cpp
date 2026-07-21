#include "ResourceHandle.h"

#include "ResourceManager.h"

namespace Oyl::Internal
{
	ResourceHandleBase::ResourceHandleBase(ResourceTypeId a_typeId)
		: m_type(a_typeId) {}

	ResourceHandleBase::ResourceHandleBase(ResourceTypeId a_type, ResourceId a_id, ResourceManager* a_manager)
		: ResourceHandleBase(a_type)
	{
		m_id = a_id;
		m_resourceManager = a_manager;

		if (m_id == ResourceId::Null || !m_resourceManager)
			return;

		m_resourceManager->IncrementResourceRef(m_type, m_id);
	}

	ResourceHandleBase&
	ResourceHandleBase::operator =(const ResourceHandleBase& a_other)
	{
		if (*this == a_other)
			return *this;

		OYL_ASSERT(m_type == a_other.m_type);

		if (a_other.m_id != ResourceId::Null)
			a_other.m_resourceManager->IncrementResourceRef(a_other.m_type, a_other.m_id);

		if (m_id != ResourceId::Null)
			m_resourceManager->Destroy(*this);

		m_id = a_other.m_id;
		m_resourceManager = a_other.m_resourceManager;

		return *this;
	}

	ResourceHandleBase&
	ResourceHandleBase::operator =(ResourceHandleBase&& a_other) noexcept
	{
		if (this == &a_other)
			return *this;

		OYL_ASSERT(m_type == a_other.m_type);

		if (m_id != ResourceId::Null)
			m_resourceManager->Destroy(*this);

		m_id = a_other.m_id;
		m_resourceManager = a_other.m_resourceManager;

		a_other.m_id = ResourceId::Null;

		return *this;
	}

	bool
	operator ==(const ResourceHandleBase& a_lhs, const ResourceHandleBase& a_rhs)
	{
		if (a_lhs.m_type != a_rhs.m_type)
			return false;

		if (a_lhs.m_id != a_rhs.m_id)
			return false;

		return true;
	}

	ResourceHandleBase::~ResourceHandleBase()
	{
		if (m_id == ResourceId::Null)
			return;

		Release();
	}

	ResourceBase*
	ResourceHandleBase::Get()
	{
		assert(m_resourceManager && m_type != ResourceTypeId::Null);

		if (m_id == ResourceId::Null)
			return nullptr;

		return m_resourceManager->GetResource(m_type, m_id);
	}

	void
	ResourceHandleBase::Release()
	{
		m_resourceManager->Destroy(*this);
		m_id = ResourceId::Null;
	}
}
