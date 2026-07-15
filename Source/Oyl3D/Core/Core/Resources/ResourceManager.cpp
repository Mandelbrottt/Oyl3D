#include "ResourceManager.h"

#include <ranges>

#include "Resource.h"

namespace Oyl::Internal
{
	struct ResourceData
	{
		std::unique_ptr<ResourceBase> resource;
		uint32 nRefs;
	};

	struct ResourceMap
	{
		std::unordered_map<ResourceId, ResourceData> map;
		ResourceId nextId = ResourceId(1);

		ResourceId
		GetNextId()
		{
			auto rawId = static_cast<Traits::UnderlyingType_T<ResourceId>>(nextId);
			nextId = static_cast<ResourceId>(rawId + 1);
			return static_cast<ResourceId>(rawId);
		}
	};

	struct ResourceManager::Impl
	{
		std::unordered_map<ResourceTypeId, ResourceMap> resourceTypeMap;
	};

	ResourceManager::ResourceManager()
		: m_impl(std::make_unique<Impl>()) {}

	ResourceManager::~ResourceManager()
	{
		for (auto& resourceMap : m_impl->resourceTypeMap | std::views::values)
		{
			for (auto& resourceRef : resourceMap.map | std::views::values)
			{
				OYL_ASSERT(
					!resourceRef.resource->IsLoaded(),
					"All resources should be unloaded before resource manager is destroyed!"
				);
			}
		}
	}

	void
	ResourceManager::Destroy(const ResourceHandleBase& a_handle)
	{
		if (a_handle.m_id == ResourceId::Null)
			return;

		// If the handle exists, we know a ResourceMap exists for the given type
		auto resourceMapIter = m_impl->resourceTypeMap.find(a_handle.m_type);
		auto& resourcesMap = resourceMapIter->second.map;
		auto resourceIter = resourcesMap.find(a_handle.m_id);
		OYL_ASSERT(resourceIter != resourcesMap.end());

		auto& resourceData = resourceIter->second;

		// Decrement the ref count, assure it's not already 0
		OYL_ASSERT(resourceData.nRefs > 0);
		resourceData.nRefs--;

		// If there are still references, don't destroy the backing resource
		if (resourceData.nRefs > 0)
			return;

		resourceData.resource->Unload();

		resourcesMap.erase(resourceIter);
	}

	ResourceId
	ResourceManager::CreateResource(ResourceTypeId a_type, std::unique_ptr<ResourceBase>&& a_resource)
	{
		// If ResourceMap for type doesn't exist, create it
		auto resourceMapIter = m_impl->resourceTypeMap.find(a_type);
		if (resourceMapIter == m_impl->resourceTypeMap.end())
		{
			resourceMapIter = m_impl->resourceTypeMap.emplace(a_type, ResourceMap {}).first;
		}

		// Create resource and assign an id
		auto& resources = resourceMapIter->second;
		auto resourceId = resources.GetNextId();
		auto resourceIter = resources.map.emplace(resourceId, ResourceData {}).first;

		// Initialize ResourceData with ref count and ptr, and load the resource
		auto& resourceData = resourceIter->second;
		resourceData.nRefs = 1;
		resourceData.resource = std::move(a_resource);
		resourceData.resource->Load();

		return resourceId;
	}

	void
	ResourceManager::IncrementResourceRef(ResourceTypeId a_type, ResourceId a_id)
	{
		// We should only have the id if the resource is valid
		auto resourceMapIter = m_impl->resourceTypeMap.find(a_type);
		OYL_ASSERT(resourceMapIter != m_impl->resourceTypeMap.end());

		auto& resourcesMap = resourceMapIter->second.map;
		auto& resourceData = resourcesMap.at(a_id);

		resourceData.nRefs++;
	}

	ResourceBase*
	ResourceManager::GetResource(ResourceTypeId a_typeId, ResourceId a_id)
	{
		// If we don't have a ResourceMap for the type, return
		auto resourceMapIter = m_impl->resourceTypeMap.find(a_typeId);
		if (resourceMapIter == m_impl->resourceTypeMap.end())
			return nullptr;

		// If no resource with the given id exists, return
		auto& resourcesMap = resourceMapIter->second.map;
		auto resourceIter = resourcesMap.find(a_id);
		if (resourceIter == resourcesMap.end())
			return nullptr;

		// We found it!
		auto& resourcePtr = resourceIter->second.resource;
		return resourcePtr.get();
	}
}
