#pragma once

#include "Core/Resources/ResourceHandle.h"

namespace Oyl::Internal
{
	class ResourceManager final
	{
		friend ResourceHandleBase;

	public:
		ResourceManager();

		~ResourceManager();

		template<typename T>
		ResourceHandle<T>
		CreateHandle(ResourceId a_id = ResourceId::Null);

		void
		DestroyHandle(ResourceHandleBase& a_handle);

	private:
		ResourceId
		CreateResource(ResourceTypeId a_type, std::unique_ptr<Resource>&& a_resource);

		void
		IncrementResourceRef(ResourceTypeId a_type, ResourceId a_id);

		Resource*
		GetResource(ResourceTypeId a_typeId, ResourceId a_id);

		struct Impl;
		Impl* m_impl;
	};

	template<typename TResource>
	ResourceHandle<TResource>
	ResourceManager::CreateHandle(ResourceId a_id)
	{
		constexpr auto typeId = Reflection::GetTypeId<TResource>();

		if (a_id == ResourceId::Null)
		{
			auto resource = std::make_unique<TResource>();
			auto resourceId = CreateResource(typeId, std::move(resource));
			return ResourceHandle<TResource>(resourceId, this);
		}

		IncrementResourceRef(typeId, a_id);
		return ResourceHandle<TResource>(a_id, this);
	}
}
