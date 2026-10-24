#pragma once

#include <unordered_set>

#include <Core/Preprocessor/Common.h>
#include <Core/Types/TypeId.h>

namespace Oyl
{
	class SetBase
	{
	protected:
		explicit
		constexpr
		SetBase(TypeId a_elementTypeId)
			: m_elementTypeId(a_elementTypeId) {}

	public:
		constexpr
		TypeId
		ElementTypeId() const { return m_elementTypeId; }

	private:
		TypeId m_elementTypeId;
	};

	template<typename TElement>
	class ISet
	{
	protected:
		constexpr
		ISet() = default;

		virtual
		constexpr
		~ISet() = 0;

	public:
		using ElementTy = TElement;
		using SizeTy = uint32;

		constexpr TypeId
		ElementTypeId() const { return typeidof(TElement); }

		virtual
		constexpr void
		Add(const ElementTy& a_element) = 0;

		virtual
		constexpr void
		Add(ElementTy&& a_element) = 0;

		virtual
		bool
		Contains(const ElementTy& a_element) = 0;

		virtual
		constexpr void
		Erase(const ElementTy& a_element) = 0;
	};

	template<typename TElement>
	ISet<TElement>::~ISet() {}

	template<typename TElement>
	class Set : public ISet<TElement>, public SetBase
	{
	public:
		using ElementTy = TElement;

		constexpr
		Set()
			: SetBase(typeidof(ElementTy)) {}

		using ISet<TElement>::ElementTypeId;

		void
		Add(const ElementTy& a_element) override
		{
			m_set.insert(a_element);
		}

		void
		Add(ElementTy&& a_element) override
		{
			m_set.insert(std::move(a_element));
		}

		bool
		Contains(const ElementTy& a_element) override
		{
			auto iter = m_set.find(a_element);
			return iter != m_set.end();
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
			for (const auto& element : m_set)
				if (element == a_element)
					return true;
			return false;
		}

		constexpr void
		Erase(const ElementTy& a_element) override
		{
			m_set.erase(a_element);
		}

	private:
		std::unordered_set<ElementTy> m_set;
	};
}
