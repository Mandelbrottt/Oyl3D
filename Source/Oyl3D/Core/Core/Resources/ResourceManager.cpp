#include "ResourceManager.h"

#include "Resource.h"

namespace Oyl::Internal
{
	struct ResourceRefCount
	{
		std::unique_ptr<Resource> resource;
		uint32 nRefs;
	};

	using ResourceMap = std::unordered_map<ResourceId, ResourceRefCount>;

	struct ResourceManager::Impl
	{
		std::unordered_map<ResourceTypeId, ResourceMap> resourceTypeMap;
	};

	ResourceManager::ResourceManager()
		: m_impl(new Impl) {}

	ResourceManager::~ResourceManager()
	{
		for (auto& [type, resourceMap] : m_impl->resourceTypeMap)
		{
			for (auto& [id, resourceRef] : resourceMap)
			{
				OYL_ASSERT(
					!resourceRef.resource->IsLoaded(),
					"All resources should be unloaded before resource manager is destroyed!"
				)
			}
		}
	}

	ResourceHandleBase
	ResourceManager::CreateHandle(ResourceTypeId a_typeId)
	{
		return TODO_IMPLEMENT_ME;
	}

	void
	ResourceManager::DestroyHandle(ResourceHandleBase& a_handle)
	{
		TODO_IMPLEMENT_ME();
	}

	Resource*
	ResourceManager::GetResource(ResourceTypeId a_typeId, ResourceId a_id)
	{
		return TODO_IMPLEMENT_ME;
	}
}
