#pragma once
#include <Rendering/RenderPass.h>

#include "Reflection/TypeId.h"

namespace Oyl
{
	class DictionaryBase
	{
	protected:
		DictionaryBase(Reflection::TypeId a_keyTypeId, Reflection::TypeId a_valueTypeId)
			: m_keyTypeId(a_keyTypeId),
			  m_valueTypeId(a_valueTypeId) {}

	private:
		Reflection::TypeId m_keyTypeId;
		Reflection::TypeId m_valueTypeId;
	};

	template<typename TKey, typename TValue>
	class Dictionary : public DictionaryBase
	{
	public:
		using KeyType = TKey;
		using ValueType = TValue;

		constexpr
		Dictionary()
			: DictionaryBase(typeidof(KeyType), typeidof(ValueType)) {}

		explicit
		Dictionary(const Dictionary& a_other)
			: Dictionary()
		{
			*this = a_other;
		}

		Dictionary&
		operator =(const Dictionary& a_other)
		{
			if (this != &a_other)
			{
				m_unorderedMap = a_other.m_unorderedMap;
			}
			return *this;
		}

		explicit
		Dictionary(Dictionary&& a_other) noexcept
			: Dictionary()
		{
			*this = a_other;
		}

		Dictionary&
		operator =(Dictionary&& a_other) noexcept
		{
			if (this != &a_other)
			{
				std::swap(m_unorderedMap, a_other.m_unorderedMap);
			}
			return *this;
		}

		~Dictionary() = default;

		constexpr bool
		ContainsKey(const KeyType& a_key)
		{
			return m_unorderedMap.contains(a_key);
		}

		void
		Assign(const KeyType& a_key, const ValueType& a_valueType)
		{
			m_unorderedMap.insert_or_assign(a_key, a_valueType);
		}

		ValueType&
		At(const KeyType& a_key)
		{
			return m_unorderedMap.at(a_key);
		}

		const ValueType&
		At(const KeyType& a_key) const
		{
			return m_unorderedMap.at(a_key);
		}

		ValueType&
		operator [](const KeyType& a_key)
		{
			return m_unorderedMap[a_key];
		}

		const ValueType&
		operator [](const KeyType& a_key) const
		{
			return m_unorderedMap[a_key];
		}

		void
		Erase(const KeyType& a_key)
		{
			m_unorderedMap.erase(a_key);
		}

	private:
		std::unordered_map<KeyType, ValueType> m_unorderedMap;
	};
}
