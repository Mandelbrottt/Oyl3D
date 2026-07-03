#pragma once

#include <cassert>

#include "Core/NameOf.h"
#include "Core/Typedefs.h"
#include "Core/TypeTraits.h"

namespace Oyl::Reflection
{
	enum class TypeId : uint64
	{
		Null = static_cast<uint64>(0)
	};

	constexpr
	bool
	operator ==(TypeId a_lhs, TypeId a_rhs)
	{
		auto lhs = static_cast<Traits::UnderlyingType_T<TypeId>>(a_lhs);
		auto rhs = static_cast<Traits::UnderlyingType_T<TypeId>>(a_rhs);

		return lhs == rhs;
	}

	constexpr
	bool
	operator !=(TypeId a_lhs, TypeId a_rhs)
	{
		return !(a_lhs == a_rhs);
	}

	constexpr
	bool
	operator ==(TypeId a_lhs, Traits::UnderlyingType_T<TypeId> a_rhs)
	{
		return a_lhs == static_cast<TypeId>(a_rhs);
	}

	constexpr
	bool
	operator !=(TypeId a_lhs, Traits::UnderlyingType_T<TypeId> a_rhs)
	{
		return !(a_lhs == a_rhs);
	}

	constexpr
	bool
	operator ==(Traits::UnderlyingType_T<TypeId> a_lhs, TypeId a_rhs)
	{
		return a_rhs == a_lhs;
	}

	constexpr
	bool
	operator !=(Traits::UnderlyingType_T<TypeId> a_lhs, TypeId a_rhs)
	{
		return !(a_lhs == a_rhs);
	}

	namespace Detail
	{
		// Hash algorithm from http://isthe.com/chongo/tech/comp/fnv/#FNV-1a

		template<typename T>
			requires (sizeof(T) == 4)
		constexpr T
		Hash(std::string_view a_view)
		{
			T hash = 2166136261UL;
			for (size_t i = 0; i < a_view.size(); i++)
			{
				char octet = a_view.at(i);
				hash = hash xor octet;
				hash = hash * 16777619UL;
			}
			return hash;
		}

		template<typename T>
			requires (sizeof(T) == 8)
		constexpr T
		Hash(std::string_view a_view)
		{
			T hash = 14695981039346656037ULL;
			for (size_t i = 0; i < a_view.size(); i++)
			{
				char octet = a_view.at(i);
				hash = hash xor octet;
				hash = hash * 1099511628211ULL;
			}
			return hash;
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
		requires (!Traits::PointerToPointer<T>)
	constexpr TypeId
	GetTypeId() noexcept
	{
		using TRaw = std::decay_t<Traits::RemovePointer<T>>;

		using TUnderlying = Traits::UnderlyingType_T<TypeId>;

		// We use a hash instead of an incrementing counter to determine types because calls from different
		// compilation targets (libraries) would result in different values with the same type at some levels
		// of optimization.
		// We need a custom implemented hash function because std::hash isn't constexpr in c++20
		// Generally hashes are much more consistent and chance of cache hit is very small

		// Compute hash based on the typename
		std::string_view name = NameOf<TRaw>();
		TUnderlying hash = Detail::Hash<TUnderlying>(name);

		assert(hash != TypeId::Null);

		return static_cast<TypeId>(hash);
	}

	#define typeidof(_type_) ::Oyl::Reflection::GetTypeId<_type_>()
}
