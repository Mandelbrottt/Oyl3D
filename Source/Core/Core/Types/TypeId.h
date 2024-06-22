#pragma once

namespace Oyl
{
	using type_id_underlying_t = u32;

	enum class TypeId : type_id_underlying_t { Null = 0, FirstValid = 1 };

	constexpr
	bool
	operator ==(TypeId a_lhs, TypeId a_rhs)
	{
		auto lhs = static_cast<type_id_underlying_t>(a_lhs);
		auto rhs = static_cast<type_id_underlying_t>(a_rhs);

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
		template<typename T>
		using raw_type_t = std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
		
		template<typename T, typename = std::enable_if_t<std::is_same_v<T, raw_type_t<T>>>>
		static
		TypeId
		GetRawTypeId() noexcept
		{
			static TypeId result = []()
			{
				// Compute hash based on the typename
				std::hash<std::string_view> hashFn;

				auto& type = typeid(T);
				const char* name = type.name();
				//const char* raw_name = type.raw_name();
				std::size_t hash = hashFn(name);

				OYL_ASSERT(hash != 0);
				
				OYL_LOG_DEBUG("Computing hash for type \"{}\": {}", name, hash);
				
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
		// Abstract call to GetRawTypeId so that each ID hash is only computed once in the static initializer
		return Detail::GetRawTypeId<Detail::raw_type_t<T>>();
	}
}