#pragma once

#include "Core/Resources/Resource.h"

namespace Oyl
{
	enum class ResourceId : uint32 { Null = std::numeric_limits<uint32>() };

	namespace Internal
	{
		class ResourceManager;

		class ResourceHandleBase
		{
		protected:
			ResourceHandleBase(ResourceTypeId a_typeId, ResourceManager* a_manager);

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

		private:
			ResourceId m_id = ResourceId::Null;

			ResourceTypeId m_type;
			ResourceManager* m_resourceManager; // TODO: Make Global, or use locator?
		};
	}

	template<typename TResource>
	class ResourceHandle : Internal::ResourceHandleBase
	{
	public:
		ResourceHandle()
			: ResourceHandleBase(Reflection::GetTypeId<TResource>()) {}

		virtual
		~ResourceHandle() {}

		TResource*
		Get() final
		{
			return static_cast<TResource*>(ResourceHandleBase::Get());
		}

		const TResource*
		Get() const final
		{
			return static_cast<const TResource*>(ResourceHandleBase::Get());
		}

		TResource*
		operator ->()
		{
			return Get();
		}
	};
}
