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
		ResourceHandle<TResource>
		CreateHandle();

		template<typename TResource>
		ResourceHandle<TResource>
		CreateHandle(ResourceId a_id);

		void
		DestroyHandle(const ResourceHandleBase& a_handle);

	private:
		template<typename TResource>
		ResourceId
		CreateResource();

		ResourceId
		CreateResource(ResourceTypeId a_type, std::unique_ptr<Resource>&& a_resource);

		void
		IncrementResourceRef(ResourceTypeId a_type, ResourceId a_id);

		Resource*
		GetResource(ResourceTypeId a_typeId, ResourceId a_id);

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	template<typename TResource>
	ResourceId
	ResourceManager::CreateResource()
	{
		constexpr auto typeId = Reflection::GetTypeId<TResource>();
		auto resourcePtr = std::make_unique<TResource>();
		return CreateResource(typeId, std::move(resourcePtr));
	}

	template<typename TResource>
	ResourceHandle<TResource>
	ResourceManager::CreateHandle()
	{
		constexpr auto typeId = Reflection::GetTypeId<TResource>();
		auto resourceId = CreateResource<TResource>();
		return ResourceHandle<TResource>(resourceId, this);
	}

	template<typename TResource>
	ResourceHandle<TResource>
	ResourceManager::CreateHandle(ResourceId a_id)
	{
		OYL_ASSERT(a_id != ResourceId::Null);

		constexpr auto typeId = Reflection::GetTypeId<TResource>();
		IncrementResourceRef(typeId, a_id);
		return ResourceHandle<TResource>(a_id, this);
	}
}
