#pragma once

#include "Core/Resources/Resource.h"

namespace Oyl
{
	enum class ResourceId : uint32
	{
		Null = 0,
		FirstValid = 1
	};

	namespace Internal
	{
		class ResourceManager;

		class ResourceHandleBase
		{
			friend ResourceManager;

		protected:
			explicit
			ResourceHandleBase(ResourceTypeId a_typeId);

			ResourceHandleBase(ResourceTypeId a_type, ResourceId a_id, ResourceManager* a_manager);

		public:
			virtual
			~ResourceHandleBase();

			virtual
			Resource*
			Get();

			virtual
			const Resource*
			Get() const
			{
				return const_cast<ResourceHandleBase*>(this)->Get();
			}

			ResourceId
			GetResourceId() const
			{
				return m_id;
			}

		private:
			ResourceTypeId m_type;

			ResourceId m_id = ResourceId::Null;
			ResourceManager* m_resourceManager = nullptr;; // TODO: Make Global, or use locator?
		};
	}

	template<typename TResource>
	class ResourceHandle final : Internal::ResourceHandleBase
	{
	public:
		ResourceHandle()
			: ResourceHandleBase(Reflection::GetTypeId<TResource>())
		{
			static_assert(sizeof(ResourceHandle) == sizeof(ResourceHandleBase));
		}

		TResource*
		Get()
		{
			return static_cast<TResource*>(ResourceHandleBase::Get());
		}

		const TResource*
		Get() const
		{
			return static_cast<const TResource*>(ResourceHandleBase::Get());
		}

		TResource*
		operator ->()
		{
			return Get();
		}

	private:
		ResourceHandle(ResourceId a_id, Internal::ResourceManager* a_manager)
			: ResourceHandleBase(Reflection::GetTypeId<TResource>(), a_id, a_manager) {}
	};

}
