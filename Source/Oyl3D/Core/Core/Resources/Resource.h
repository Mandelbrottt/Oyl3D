#pragma once

#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	using ResourceTypeId = Reflection::TypeId;

	namespace Internal
	{
		class OYL_CORE_API ResourceBase
		{
		protected:
			ResourceBase();

		public:
			virtual
			~ResourceBase();

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
			IsLoaded() const;

		private:
			bool m_loaded;
		};
	}

	template<typename TResource>
	class Resource : public Internal::ResourceBase
	{
	protected:
		Resource() = default;

	public:
		static
		constexpr ResourceTypeId
		GetResourceTypeId()
		{
			static_assert(std::is_convertible_v<TResource*, Resource*>);
			return Reflection::GetTypeId<TResource>();
		}
	};

	template<typename TResource>
	class CompilableResource : public Resource<TResource>
	{
	protected:
		CompilableResource()
			: m_dirty(true) {}

		explicit
		CompilableResource(bool a_dirty)
			: m_dirty(a_dirty) {}

	public:
		bool
		IsDirty() const
		{
			return m_dirty;
		}

	protected:
		void
		SetDirty()
		{
			m_dirty = true;
		}

		virtual
		bool
		Compile()
		{
			m_dirty = false;
			return m_dirty;
		}

	private:
		bool m_dirty;
	};

	namespace Traits
	{
		// Don't check against Resource<T>, since we allow resource subclasses
		template<typename T>
		struct IsResource
		{
			// Enforce that T is not a direct descendant of Internal::ResourceBase
			static constexpr bool value =
				std::is_convertible_v<T*, Internal::ResourceBase*>
				&& std::is_base_of_v<Internal::ResourceBase, T>;

			static_assert(
				&T::GetResourceTypeId,
				"Resource type must implement static constexpr ResourceTypeId GetResourceTypeId()!"
			);
		};

		template<typename T>
		constexpr bool IsResource_V = IsResource<T>::value;
	}
}
