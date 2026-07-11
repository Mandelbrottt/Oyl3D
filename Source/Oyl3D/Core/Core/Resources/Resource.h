#pragma once

#include "Core/Reflection/TypeId.h"

namespace Oyl
{
	using ResourceTypeId = Reflection::TypeId;

	class OYL_CORE_API Resource
	{
	protected:
		Resource();

	public:
		virtual
		~Resource() noexcept;

		virtual
		bool
		Load() noexcept;

		virtual
		bool
		Unload() noexcept;

		bool
		IsLoaded() const noexcept;

		bool
		IsDirty() const noexcept;

	protected:
		virtual
		void
		Compile() = 0;

	private:
		bool m_loaded;
		bool m_dirty;
	};
}
