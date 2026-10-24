#pragma once

#include <unordered_map>

#include <Core/Preprocessor/Common.h>
#include <Core/Types/TypeId.h>

namespace Oyl
{
	template<typename TKey, typename TValue>
	class IMap
	{
	protected:
		constexpr
		IMap() = default;

		virtual
		constexpr
		~IMap() = 0;

	public:
		using KeyTy = TKey;
		using ValueTy = TValue;
		using SizeTy = uint32;

		constexpr TypeId
		KeyTypeId() const { return typeidof(KeyTy); }

		constexpr TypeId
		ValueTypeId() const { return typeidof(ValueTy); }

		virtual
		constexpr
		SizeTy
		Size() const = 0;

		virtual
		constexpr SizeTy
		Count() const { return Size(); };

		virtual
		constexpr bool
		ContainsKey(const KeyTy& a_key) const = 0;

		virtual
		constexpr void
		Assign(const KeyTy& a_key, ValueTy&& a_valueType) = 0;

		virtual
		constexpr ValueTy&
		At(const KeyTy& a_key) = 0;

		virtual
		constexpr const ValueTy&
		At(const KeyTy& a_key) const = 0;

		virtual
		constexpr ValueTy&
		operator [](const KeyTy& a_key) = 0;

		virtual
		constexpr const ValueTy&
		operator [](const KeyTy& a_key) const = 0;

		virtual
		constexpr void
		Erase(const KeyTy& a_key) = 0;

		virtual
		constexpr void
		Clear() = 0;
	};

	template<typename TKey, typename TValue>
	IMap<TKey, TValue>::~IMap() {}

	template<typename TKey, typename TValue>
	class Map : public IMap<TKey, TValue>
	{
	public:
		using KeyTy = typename IMap<TKey, TValue>::KeyTy;
		using ValueTy = typename IMap<TKey, TValue>::ValueTy;
		using SizeTy = typename IMap<TKey, TValue>::SizeTy;

		constexpr
		Map() = default;

		explicit
		constexpr
		Map(const Map& a_other)
			: Map()
		{
			*this = a_other;
		}

		constexpr Map&
		operator =(const Map& a_other)
		{
			if (this != &a_other)
			{
				m_unorderedMap = a_other.m_unorderedMap;
			}
			return *this;
		}

		explicit
		constexpr
		Map(Map&& a_other) noexcept
			: Map()
		{
			*this = a_other;
		}

		constexpr Map&
		operator =(Map&& a_other) noexcept
		{
			if (this != &a_other)
			{
				std::swap(m_unorderedMap, a_other.m_unorderedMap);
			}
			return *this;
		}

		virtual
		constexpr
		~Map() = default;

		constexpr SizeTy
		Size() const override
		{
			return m_unorderedMap.size();
		}

		constexpr bool
		ContainsKey(const KeyTy& a_key) const override
		{
			return m_unorderedMap.contains(a_key);
		}

		void
		Assign(const KeyTy& a_key, ValueTy&& a_valueType) override
		{
			m_unorderedMap.insert_or_assign(a_key, std::move(a_valueType));
		}

		ValueTy&
		At(const KeyTy& a_key) override
		{
			return m_unorderedMap.at(a_key);
		}

		const ValueTy&
		At(const KeyTy& a_key) const override
		{
			return m_unorderedMap.at(a_key);
		}

		ValueTy&
		operator [](const KeyTy& a_key) override
		{
			return m_unorderedMap[a_key];
		}

		const ValueTy&
		operator [](const KeyTy& a_key) const override
		{
			return m_unorderedMap[a_key];
		}

		void
		Erase(const KeyTy& a_key) override
		{
			m_unorderedMap.erase(a_key);
		}

		constexpr
		void
		Clear() override
		{
			m_unorderedMap.clear();
		}

	private:
		std::unordered_map<KeyTy, ValueTy> m_unorderedMap;
	};
}
