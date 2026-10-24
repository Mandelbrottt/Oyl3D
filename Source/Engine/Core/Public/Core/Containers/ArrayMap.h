#pragma once

#include "Array.h"
#include "Map.h"

namespace Oyl
{
	template<typename TKey, typename TValue>
	class ArrayMap : public IMap<TKey, TValue>
	{
	public:
		using KeyTy = typename IMap<TKey, TValue>::KeyTy;
		using ValueTy = typename IMap<TKey, TValue>::ValueTy;
		using SizeTy = typename IMap<TKey, TValue>::SizeTy;

		struct Pair
		{
			KeyTy key;
			ValueTy value;
		};

		constexpr
		ArrayMap() {}

		constexpr SizeTy
		Size() const override
		{
			return m_array.Size();
		}

		constexpr bool
		ContainsKey(const KeyTy& a_key) const override
		{
			for (const auto& [key, value] : m_array)
				if (a_key == key)
					return true;
			return false;
		}

		constexpr void
		Assign(const KeyTy& a_key, ValueTy&& a_valueType) override
		{
			At(a_key) = std::move(a_valueType);
		}

		ValueTy&
		At(const KeyTy& a_key) override
		{
			for (auto& [key, value] : m_array)
				if (key == a_key)
					return value;

			auto& kvp = m_array.emplace_back(a_key, ValueTy());
			return kvp.value;
		}

		const ValueTy&
		At(const KeyTy& a_key) const override
		{
			return const_cast<ArrayMap*>(this)->At(a_key);
		}

		ValueTy&
		operator [](const KeyTy& a_key) override { return At(a_key); }

		const TValue&
		operator [](const KeyTy& a_key) const override { return At(a_key); }

		template<typename... TArgs>
		ValueTy&
		Emplace(const KeyTy& a_key, TArgs&&... a_args)
		{
			auto& value = operator[](a_key);
			new(&value) ValueTy(std::forward<TArgs>(a_args)...);
			return value;
		}

		void
		Erase(const KeyTy& a_key) override
		{
			for (uint32 i = 0; i < m_array.Length(); i++)
				if (m_array[i].key == a_key)
				{
					m_array.erase(m_array.begin() + i);
					return;
				}
		}

		void
		Clear() override
		{
			m_array.Clear();
		}

		void
		Reserve(SizeTy a_size)
		{
			m_array.Reserve(a_size);
		}

		typename Array<Pair>::iterator
		begin() { return m_array.begin(); }

		typename Array<Pair>::const_iterator
		begin() const { return m_array.begin(); }

		typename Array<Pair>::iterator
		end() { return m_array.end(); }

		typename Array<Pair>::const_iterator
		end() const { return m_array.end(); }

	private:
		Array<Pair> m_array;
	};
}
