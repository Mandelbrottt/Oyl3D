#pragma once
#include "Reflection/TypeId.h"

namespace Oyl
{
	class SetBase
	{
	protected:
		explicit
		SetBase(Reflection::TypeId a_elementTypeId)
			: m_elementTypeId(a_elementTypeId) {}

	private:
		Reflection::TypeId m_elementTypeId;
	};

	template<typename TElement>
	class Set : public SetBase
	{
	public:
		using ElementType = TElement;

		Set()
			: SetBase(typeidof(ElementType)) {}

		void
		Add(const ElementType& a_element)
		{
			m_set.insert(a_element);
		}

		void
		Add(ElementType&& a_element)
		{
			m_set.insert(std::move(a_element));
		}

		bool
		Contains(const ElementType& a_element)
		{
			auto iter = m_set.find(a_element);
			return iter != m_set.end();
		}

	private:
		std::unordered_set<ElementType> m_set;
	};
}
