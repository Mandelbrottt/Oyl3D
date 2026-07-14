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

		template<typename TResource, typename... TArgs>
			requires (Traits::IsResource_V<TResource>)
		ResourceHandle<TResource>
		Load(TArgs&&... a_args);

		template<typename TResource>
			requires (Traits::IsResource_V<TResource>)
		ResourceHandle<TResource>
		Get(ResourceId a_id);

		void
		Destroy(const ResourceHandleBase& a_handle);

	private:
		template<typename TResource, typename... TArgs>
			requires (Traits::IsResource_V<TResource>)
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

	template<typename TResource, typename... TArgs>
		requires (Traits::IsResource_V<TResource>)
	ResourceHandle<TResource>
	ResourceManager::Load(TArgs&&... a_args)
	{
		auto resourceId = CreateResource<TResource>(std::forward<TArgs>(a_args)...);
		return ResourceHandle<TResource>(resourceId, this);
	}

	template<typename TResource>
		requires (Traits::IsResource_V<TResource>)
	ResourceHandle<TResource>
	ResourceManager::Get(ResourceId a_id)
	{
		OYL_ASSERT(a_id != ResourceId::Null);

		constexpr auto typeId = TResource::GetResourceTypeId();
		IncrementResourceRef(typeId, a_id);
		return ResourceHandle<TResource>(a_id, this);
	}

	template<typename TResource, typename... TArgs>
		requires (Traits::IsResource_V<TResource>)
	ResourceId
	ResourceManager::CreateResource(TArgs&&... a_args)
	{
		constexpr auto typeId = TResource::GetResourceTypeId();
		auto resourcePtr = std::make_unique<TResource>(std::forward<TArgs>(a_args)...);
		return CreateResource(typeId, std::move(resourcePtr));
	}

}
