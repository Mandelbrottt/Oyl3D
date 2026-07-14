#pragma once

#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	using ResourceTypeId = Reflection::TypeId;

	class OYL_CORE_API ResourceBase
	{
	protected:
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

		bool
		IsDirty() const noexcept;

	protected:
		virtual
		bool
		Compile(void* a_customData);

		void
		SetDirty() noexcept;

	private:
		bool m_loaded;
		bool m_dirty;
	};

	template<typename TResource>
	class Resource : public ResourceBase
	{
	public:
		static
		constexpr ResourceTypeId
		GetResourceTypeId()
		{
			static_assert(std::is_base_of_v<Resource, TResource>);
			return Reflection::GetTypeId<TResource>();
		}
	};
}
