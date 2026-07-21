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

		template<Traits::Resource TResource, typename... TArgs>
		ResourceHandle<TResource>
		Load(TArgs&&... a_args);

		template<Traits::ResourceHandle TResourceHandle, typename... TArgs>
		TResourceHandle
		Load(TArgs&&... a_args);

		template<Traits::Resource TResource>
		ResourceHandle<TResource>
		Get(ResourceId a_id) const;

		template<Traits::ResourceHandle TResourceHandle>
		TResourceHandle
		Get(ResourceId a_id) const;

		void
		Destroy(const ResourceHandleBase& a_handle);

	private:
		template<Traits::Resource TResource, typename... TArgs>
		ResourceId
		CreateResource(TArgs&&... a_args);

		ResourceId
		CreateResource(ResourceTypeId a_type, std::unique_ptr<ResourceBase>&& a_resource);

		void
		IncrementResourceRef(ResourceTypeId a_type, ResourceId a_id);

		ResourceBase*
		GetResource(ResourceTypeId a_typeId, ResourceId a_id);

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	template<Traits::Resource TResource, typename... TArgs>
	ResourceHandle<TResource>
	ResourceManager::Load(TArgs&&... a_args)
	{
		auto resourceId = CreateResource<TResource>(std::forward<TArgs>(a_args)...);
		return ResourceHandle<TResource>(ResourceHandleBase::Tag {}, resourceId, this);
	}

	template<Traits::ResourceHandle TResourceHandle, typename... TArgs>
	TResourceHandle
	ResourceManager::Load(TArgs&&... a_args)
	{
		return Load<typename TResourceHandle::ResourceType>(std::forward<TArgs>(a_args)...);
	}

	template<Traits::Resource TResource>
	ResourceHandle<TResource>
	ResourceManager::Get(ResourceId a_id) const
	{
		OYL_ASSERT(a_id != ResourceId::Null);

		constexpr auto typeId = TResource::GetResourceTypeId();
		IncrementResourceRef(typeId, a_id);
		return ResourceHandle<TResource>(ResourceHandleBase::Tag {}, a_id, this);
	}

	template<Traits::ResourceHandle TResourceHandle>
	TResourceHandle
	ResourceManager::Get(ResourceId a_id) const
	{
		return Get<typename TResourceHandle::ResourceType>(a_id);
	}

	template<Traits::Resource TResource, typename... TArgs>
	ResourceId
	ResourceManager::CreateResource(TArgs&&... a_args)
	{
		constexpr auto typeId = TResource::GetResourceTypeId();
		auto resourcePtr = std::make_unique<TResource>(std::forward<TArgs>(a_args)...);
		return CreateResource(typeId, std::move(resourcePtr));
	}
}
