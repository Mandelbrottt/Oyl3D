#pragma once

#include "Array.h"

namespace Oyl
{
	template<typename TKey, typename TValue>
	class ArrayMap
	{
	public:
		using KeyTy = TKey;
		using ValueTy = TValue;
		using SizeTy = uint32;

		struct Pair
		{
			KeyTy key;
			ValueTy value;
		};

		ArrayMap() {}

		bool
		Contains(const KeyTy& a_key) const
		{
			for (const auto& [key, value] : m_array)
				if (a_key == key)
					return true;
			return false;
		}

		ValueTy&
		At(const KeyTy& a_key)
		{
			for (auto& [key, value] : m_array)
				if (key == a_key)
					return value;

			auto& kvp = m_array.emplace_back(a_key, ValueTy());
			return kvp.value;
		}

		const ValueTy&
		At(const KeyTy& a_key) const
		{
			return const_cast<ArrayMap*>(this)->At(a_key);
		}

		ValueTy&
		operator [](const KeyTy& a_key) { return At(a_key); }

		const TValue&
		operator [](const KeyTy& a_key) const { return At(a_key); }

		template<typename... TArgs>
		ValueTy&
		Emplace(const KeyTy& a_key, TArgs&&... a_args)
		{
			auto& value = operator[](a_key);
			new(&value) ValueTy(std::forward<TArgs>(a_args)...);
			return value;
		}

		void
		Erase(const KeyTy& a_key)
		{
			for (uint32 i = 0; i < m_array.Length(); i++)
				if (m_array[i].key == a_key)
				{
					m_array.erase(m_array.begin() + i);
					return;
				}
		}

		constexpr SizeTy
		Size() const
		{
			return m_array.Size();
		}

		constexpr SizeTy
		Length() const
		{
			return m_array.Length();
		}

		void
		Reserve(SizeTy a_size)
		{
			m_array.Reserve(a_size);
		}

		void
		Clear()
		{
			m_array.Clear();
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
