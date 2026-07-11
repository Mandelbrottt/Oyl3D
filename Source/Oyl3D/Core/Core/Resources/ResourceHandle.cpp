#include "ResourceHandle.h"

#include "ResourceManager.h"

namespace Oyl::Internal
{
	ResourceHandleBase::ResourceHandleBase(ResourceTypeId a_typeId, ResourceManager* a_manager)
		: m_type(a_typeId),
		  m_resourceManager(a_manager) {}

	ResourceHandleBase::ResourceHandleBase(ResourceTypeId a_type, ResourceId a_id, ResourceManager* a_manager)
		: ResourceHandleBase(a_type, a_manager)
	{
		m_id = a_id;
	}

	ResourceHandleBase::~ResourceHandleBase()
	{
		if (m_id == ResourceId::Null)
			return;

		m_resourceManager->DestroyHandle(*this);
	}

	Resource*
	ResourceHandleBase::Get()
	{
		assert(m_resourceManager && m_type != ResourceTypeId::Null);

		if (m_id == ResourceId::Null)
			return nullptr;

		return m_resourceManager->GetResource(m_type, m_id);
	}
}
