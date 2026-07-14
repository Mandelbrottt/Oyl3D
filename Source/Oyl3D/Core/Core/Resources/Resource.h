#pragma once

#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	using ResourceTypeId = Reflection::TypeId;

	template<typename>
	class Resource;

	namespace Internal
	{
		class OYL_CORE_API ResourceBase
		{
			template<typename T>
			friend class Resource;

			ResourceBase();

		public:
			virtual
			~ResourceBase() noexcept;

			virtual
			bool
			Load();

			virtual
			bool
			Unload();

			constexpr
			static
			ResourceTypeId
			GetResourceTypeId() = delete;

			bool
			IsLoaded() const noexcept;

		private:
			bool m_loaded;
		};
	}

	template<typename TResource>
	class Resource : public Internal::ResourceBase
	{
	public:
		static
		constexpr ResourceTypeId
		GetResourceTypeId()
		{
			static_assert(std::is_convertible_v<TResource*, Resource*>);
			return Reflection::GetTypeId<TResource>();
		}
	};

	namespace Traits
	{
		template<typename T>
		struct IsResource : std::is_convertible<T*, Internal::ResourceBase*>
		{
			static_assert(
				&T::GetResourceTypeId,
				"Resource type must implement static constexpr ResourceTypeId GetResourceTypeId()!"
			);
		};

		template<typename T>
		constexpr bool IsResource_V = IsResource<T>::value;
	}
}
