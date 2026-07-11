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
			Resource*
			Get();

			virtual
			const Resource*
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

		private:
			ResourceTypeId m_type;

			ResourceId m_id = ResourceId::Null;
			ResourceManager* m_resourceManager = nullptr;; // TODO: Make Global, or use locator?
		};
	}

	template<typename TResource>
	class ResourceHandle final : public Internal::ResourceHandleBase
	{
		friend Internal::ResourceManager;

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
