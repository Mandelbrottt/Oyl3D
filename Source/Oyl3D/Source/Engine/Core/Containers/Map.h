#pragma once

#include <unordered_map>

#include <Core/Preprocessor/Common.h>
#include <Core/Types/TypeId.h>

namespace Oyl
{
	class MapBase
	{
	protected:
		constexpr
		MapBase(TypeId a_keyTypeId, TypeId a_valueTypeId)
			: m_keyTypeId(a_keyTypeId),
			  m_valueTypeId(a_valueTypeId)
		{
			OYL_UNUSED(m_keyTypeId);
			OYL_UNUSED(m_valueTypeId);
		}

	private:
		TypeId m_keyTypeId;
		TypeId m_valueTypeId;
	};

	template<typename TKey, typename TValue>
	class Map : public MapBase
	{
	public:
		using KeyTy = TKey;
		using ValueTy = TValue;

		constexpr
		Map()
			: MapBase(typeidof(KeyTy), typeidof(ValueTy)) {}

		explicit
		Map(const Map& a_other)
			: Map()
		{
			*this = a_other;
		}

		Map&
		operator =(const Map& a_other)
		{
			if (this != &a_other)
			{
				m_unorderedMap = a_other.m_unorderedMap;
			}
			return *this;
		}

		explicit
		Map(Map&& a_other) noexcept
			: Map()
		{
			*this = a_other;
		}

		Map&
		operator =(Map&& a_other) noexcept
		{
			if (this != &a_other)
			{
				std::swap(m_unorderedMap, a_other.m_unorderedMap);
			}
			return *this;
		}

		~Map() = default;

		constexpr bool
		ContainsKey(const KeyTy& a_key)
		{
			return m_unorderedMap.contains(a_key);
		}

		void
		Assign(const KeyTy& a_key, const ValueTy& a_valueType)
		{
			m_unorderedMap.insert_or_assign(a_key, a_valueType);
		}

		ValueTy&
		At(const KeyTy& a_key)
		{
			return m_unorderedMap.at(a_key);
		}

		const ValueTy&
		At(const KeyTy& a_key) const
		{
			return m_unorderedMap.at(a_key);
		}

		ValueTy&
		operator [](const KeyTy& a_key)
		{
			return m_unorderedMap[a_key];
		}

		const ValueTy&
		operator [](const KeyTy& a_key) const
		{
			return m_unorderedMap[a_key];
		}

		void
		Erase(const KeyTy& a_key)
		{
			m_unorderedMap.erase(a_key);
		}

	private:
		std::unordered_map<KeyTy, ValueTy> m_unorderedMap;
	};
}
