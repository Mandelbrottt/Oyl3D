#pragma once

#include "Core/Resources/Resource.h"

namespace Oyl
{
	enum class ResourceId : uint32
	{
		Null = 0,
	};

	namespace Internal
	{
		class ResourceManager;

		class OYL_CORE_API ResourceHandleBase
		{
			friend ResourceManager;

		protected:
			explicit
			ResourceHandleBase(ResourceTypeId a_typeId);

			ResourceHandleBase(ResourceTypeId a_type, ResourceId a_id, ResourceManager* a_manager);

			struct Tag {};

		public:
			ResourceHandleBase(const ResourceHandleBase& a_other)
				: ResourceHandleBase(a_other.m_type)
			{
				*this = a_other;
			}

			ResourceHandleBase&
			operator =(const ResourceHandleBase& a_other);

			ResourceHandleBase(ResourceHandleBase&& a_other) noexcept
				: ResourceHandleBase(a_other.m_type)
			{
				*this = std::move(a_other);
			}

			ResourceHandleBase&
			operator =(ResourceHandleBase&& a_other) noexcept;

			virtual
			~ResourceHandleBase();

			friend
			bool
			operator ==(const ResourceHandleBase& a_lhs, const ResourceHandleBase& a_rhs);

			friend
			bool
			operator !=(const ResourceHandleBase& a_lhs, const ResourceHandleBase& a_rhs)
			{
				return !(a_lhs == a_rhs);
			}

			virtual
			ResourceBase*
			Get();

			virtual
			const ResourceBase*
			Get() const
			{
				return const_cast<ResourceHandleBase*>(this)->Get();
			}

			void
			Release();

			ResourceId
			GetResourceId() const
			{
				return m_id;
			}

		protected:
			ResourceTypeId m_type;

			ResourceId m_id = ResourceId::Null;
			ResourceManager* m_resourceManager = nullptr;; // TODO: Make Global, or use locator?
		};
	}

	template<Traits::Resource TResource>
	class ResourceHandle final : public Internal::ResourceHandleBase
	{
		friend Internal::ResourceManager;

	public:
		using ResourceType = TResource;

		ResourceHandle()
			: ResourceHandleBase(TResource::GetResourceTypeId())
		{
			static_assert(sizeof(ResourceHandle) == sizeof(ResourceHandleBase));
		}

		ResourceHandle(Tag, ResourceId a_id, Internal::ResourceManager* a_manager)
			: ResourceHandleBase(TResource::GetResourceTypeId(), a_id, a_manager) {}

		// Convert to Parent from Child
		template<Traits::Resource TSuperResource>
			requires (TResource::GetResourceTypeId() == TSuperResource::GetResourceTypeId())
		operator ResourceHandle<TSuperResource>()
		{
			return ResourceHandle<TSuperResource>(
				Tag {},
				m_id,
				m_resourceManager
			);
		}

		// Convert to Child from Parent
		template<Traits::Resource TSuperResource>
			requires (TResource::GetResourceTypeId() == TSuperResource::GetResourceTypeId())
		explicit
		ResourceHandle(const ResourceHandle<TSuperResource>& a_other)
			: ResourceHandleBase(a_other) {}

		// Convert to Child from Parent
		template<Traits::Resource TSuperResource>
			requires (TResource::GetResourceTypeId() == TSuperResource::GetResourceTypeId())
		explicit
		ResourceHandle(ResourceHandle<TSuperResource>&& a_other) noexcept
			: ResourceHandleBase(std::move(a_other)) {}

		TResource*
		Get() override
		{
			return static_cast<TResource*>(ResourceHandleBase::Get());
		}

		const TResource*
		Get() const override
		{
			return static_cast<const TResource*>(ResourceHandleBase::Get());
		}

		template<Traits::Resource TChildResource>
			requires (std::is_convertible_v<TChildResource*, TResource*>)
		TChildResource*
		Get()
		{
			static_assert(TResource::GetResourceTypeId() == TChildResource::GetResourceTypeId());
			return static_cast<TChildResource*>(Get());
		}

		template<Traits::Resource TChildResource>
			requires (std::is_convertible_v<TChildResource*, TResource*>)
		TChildResource*
		Get() const
		{
			return const_cast<ResourceHandle*>(this)->template Get<TChildResource>();
		}

		TResource*
		operator ->()
		{
			return Get();
		}
	};

	namespace Traits
	{
		template<typename TResourceHandle>
		concept ResourceHandle = requires
		{
			// TResourceHandle should be of the form ResourceHandle<TResourceHandle::ResourceType>
			std::is_same_v<TResourceHandle, Oyl::ResourceHandle<typename TResourceHandle::ResourceType>>;
		};
	}
}