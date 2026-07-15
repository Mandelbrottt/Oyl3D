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

			virtual
			bool
			IsDirty() const;

		protected:
			virtual
			void
			SetDirty();

		private:
			bool m_loaded = false;
			bool m_dirty = true;
		};
	}

	template<typename>
	class Resource : public Internal::ResourceBase
	{
	protected:
		Resource() = default;

	public:
		static
		constexpr ResourceTypeId
		GetResourceTypeId();
	};

	namespace Traits
	{
		// Don't check against Resource<T>, since we allow resource subclasses
		template<typename T>
		struct IsResource
		{
			// Enforce that T is not a direct descendant of Internal::ResourceBase
			static constexpr bool value =
				std::is_convertible_v<T*, Oyl::Internal::ResourceBase*>
				&& std::is_base_of_v<Oyl::Internal::ResourceBase, T>
				&& std::is_function_v<decltype(T::GetResourceTypeId)>;
		};

		template<typename T>
		constexpr bool IsResource_V = IsResource<T>::value;

		template<typename TResource>
		concept Resource = requires
		{
			std::is_convertible_v<TResource*, Internal::ResourceBase*>;
			std::is_base_of_v<Internal::ResourceBase, TResource>;
			{ &TResource::GetResourceTypeId } -> std::invocable;
		};
	}

	struct DeviceLoadParams {};

	struct DeviceUnloadParams {};

	template<typename TResource>
	class DeviceResource : public Resource<TResource>
	{
	protected:
		DeviceResource() {}

	public:
		bool
		IsDeviceLoaded() const
		{
			return m_deviceLoaded;
		}

		bool
		IsDeviceDirty() const
		{
			return m_deviceDirty;
		}

		bool
		Load() override;

		bool
		Unload() override;

	protected:
		void
		SetDirty() override;

		void
		SetDeviceDirty()
		{
			m_deviceDirty = true;
		}

		virtual
		bool
		DeviceLoad(void* a_params);

		virtual
		bool
		DeviceUnload(void* a_params);

	private:
		bool m_deviceLoaded = false;
		bool m_deviceDirty = true;
	};
}

#include "Resource.inl"
