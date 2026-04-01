#pragma once

#include <stdint.h>

namespace Spyll::Reflection
{
	namespace Detail
	{
		using TypeIdUnderlying = uint32_t;
	}

	enum class TypeId : Detail::TypeIdUnderlying { Null = static_cast<Detail::TypeIdUnderlying>(0) };

	constexpr
	bool
	operator ==(TypeId a_lhs, TypeId a_rhs)
	{
		auto lhs = static_cast<Detail::TypeIdUnderlying>(a_lhs);
		auto rhs = static_cast<Detail::TypeIdUnderlying>(a_rhs);

		return lhs == rhs;
	}

	constexpr
	bool
	operator !=(TypeId a_lhs, TypeId a_rhs)
	{
		return !(a_lhs == a_rhs);
	}

	namespace Detail
	{
		/**
		 * \remark Only to be called with a non-pointer, non-reference, non-cv-qualified type.
		 *         ie. int, float instead of int**, float const&
		 */
		template<typename T>
		static
		TypeId
		GetRawTypeId() noexcept
		{
			// We use a hash instead of an incrementing counter to determine types because calls from different
			// compilation targets (libraries) would result in different values with the same type at some levels
			// of optimization.
			// Don't use type_info.hash_code as it's not guaranteed to be consistent between compilations
			// Generally hashes are much more consistent and chance of cache hit is very small
			static TypeId result = []()
			{
				// Compute hash based on the typename
				std::hash<std::string> hashFn;

				auto& type = typeid(T);
				const char* name = type.name();
				std::size_t hash = hashFn(name);

				assert(hash != 0);
				
				return static_cast<TypeId>(hash);
			}();

			return result;
		}
	}
	
	/**
	 * \brief  Retrieve the statically assigned type id for the given type
	 * \tparam T The type who's Id to retrieve
	 * \return A non-zero type id for all types
	 * \remark returns the same TypeId for two types that decay to the same type.
	 *         ie. GetTypeId<const int> and GetTypeId<int> return the same id.
	 */
	template<typename T>
	TypeId
	GetTypeId() noexcept
	{
		typedef std::remove_pointer_t<std::decay_t<T>> raw_t;
		return Detail::GetRawTypeId<raw_t>();
	}
}
