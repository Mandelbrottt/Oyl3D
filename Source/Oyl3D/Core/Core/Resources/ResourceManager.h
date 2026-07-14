#pragma once

#include "Core/Resources/ResourceHandle.h"

namespace Oyl::Internal
{
	class OYL_CORE_API ResourceManager
	{
		friend ResourceHandleBase;

	public:
		ResourceManager();

		~ResourceManager();

		template<typename TResource>
			requires (std::is_base_of_v<Resource<TResource>, TResource>)
		ResourceHandle<TResource>
		CreateHandle();

		template<typename TResource>
			requires (std::is_base_of_v<Resource<TResource>, TResource>)
		ResourceHandle<TResource>
		CreateHandle(ResourceId a_id);

		void
		DestroyHandle(const ResourceHandleBase& a_handle);

	private:
		template<typename TResource>
			requires (std::is_base_of_v<Resource<TResource>, TResource>)
		ResourceId
		CreateResource();

		ResourceId
		CreateResource(ResourceTypeId a_type, std::unique_ptr<ResourceBase>&& a_resource);

		void
		IncrementResourceRef(ResourceTypeId a_type, ResourceId a_id);

		ResourceBase*
		GetResource(ResourceTypeId a_typeId, ResourceId a_id);

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	template<typename TResource>
		requires (std::is_base_of_v<Resource<TResource>, TResource>)
	ResourceId
	ResourceManager::CreateResource()
	{
		constexpr auto typeId = TResource::GetResourceTypeId();
		auto resourcePtr = std::make_unique<TResource>();
		return CreateResource(typeId, std::move(resourcePtr));
	}

	template<typename TResource>
		requires (std::is_base_of_v<Resource<TResource>, TResource>)
	ResourceHandle<TResource>
	ResourceManager::CreateHandle()
	{
		auto resourceId = CreateResource<TResource>();
		return ResourceHandle<TResource>(resourceId, this);
	}

	template<typename TResource>
		requires (std::is_base_of_v<Resource<TResource>, TResource>)
	ResourceHandle<TResource>
	ResourceManager::CreateHandle(ResourceId a_id)
	{
		OYL_ASSERT(a_id != ResourceId::Null);

		constexpr auto typeId = TResource::GetResourceTypeId();
		IncrementResourceRef(typeId, a_id);
		return ResourceHandle<TResource>(a_id, this);
	}
}
