#pragma once

#include "Array.h"
#include "Set.h"

namespace Oyl
{
	template<typename TElement>
	class ArraySet : public ISet<TElement>, public SetBase
	{
	public:
		using ElementTy = typename ISet<TElement>::ElementTy;
		using SizeTy = typename ISet<TElement>::SizeTy;

		ArraySet()
			: SetBase(typeidof(TElement)) {}

		DEFAULT_COPY(ArraySet);
		DEFAULT_MOVE(ArraySet);

		virtual
		~ArraySet() = default;

		using ISet<TElement>::ElementTypeId;

		constexpr void
		Add(const ElementTy& a_element) override
		{
			for (int i = 0; i < m_array.size(); i++)
				if (m_array[i] == a_element)
					return;
			m_array.Add(a_element);
		}

		constexpr void
		Add(ElementTy&& a_element) override
		{
			for (int i = 0; i < m_array.size(); i++)
				if (m_array[i] == a_element)
					return;
			m_array.Add(std::move(a_element));
		}

		bool
		Contains(const ElementTy& a_element) override
		{
			return Contains<ElementTy>(a_element);
		}

		template<typename UElement>
			requires requires(ElementTy a_lhs, UElement a_rhs)
			{
				{ a_lhs == a_rhs };
				{ a_lhs != a_rhs };
			}
		bool
		Contains(const UElement& a_element)
		{
			for (int i = 0; i < m_array.size(); i++)
				if (m_array[i] == a_element)
					return true;
			return false;
		}

		constexpr void
		Erase(const ElementTy& a_element) override
		{
			Erase<ElementTy>(a_element);
		}

		template<typename UElement>
			requires requires(ElementTy a_lhs, UElement a_rhs)
			{
				{ a_lhs == a_rhs };
				{ a_lhs != a_rhs };
			}
		constexpr void
		Erase(const UElement& a_element)
		{
			for (int i = 0; i < m_array.size(); i++)
				if (m_array[i] == a_element)
				{
					m_array.erase(m_array.begin() + i);
					return;
				}
		}

		typename std::vector<ElementTy>::iterator
		begin() { return m_array.begin(); }

		typename std::vector<ElementTy>::const_iterator
		begin() const { return m_array.begin(); }

		typename std::vector<ElementTy>::iterator
		end() { return m_array.end(); }

		typename std::vector<ElementTy>::const_iterator
		end() const { return m_array.end(); }

	private:
		Array<ElementTy> m_array;
	};
}