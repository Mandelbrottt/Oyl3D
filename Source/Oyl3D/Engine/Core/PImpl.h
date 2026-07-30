#pragma once

#include <memory>

namespace Oyl
{
	template<typename TImpl>
	struct PImpl
	{
		PImpl(std::nullptr_t)
			: m_impl(nullptr) {}

		explicit
		PImpl(std::unique_ptr<TImpl>&& a_impl)
			: m_impl(std::move(a_impl)) {}

		PImpl&
		operator =(PImpl&& a_other) noexcept
		{
			std::swap(m_impl, a_other.m_impl);
			return *this;
		}

		template<typename TChild>
			requires std::is_convertible_v<std::add_pointer_t<TImpl>, std::add_pointer_t<TChild>>
		explicit
		PImpl(PImpl<TChild>&& a_other)
			: m_impl(std::move(a_other.m_impl)) {}

		template<typename TChild>
			requires std::is_convertible_v<std::add_pointer_t<TImpl>, std::add_pointer_t<TChild>>
		PImpl&
		operator =(PImpl<TChild>&& a_other)
		{
			std::swap(m_impl, a_other.m_impl);
			return *this;
		}

		TImpl*
		Get() { return m_impl.get(); }

		const TImpl*
		Get() const { return m_impl.get(); }

		TImpl*
		operator ->() { return Get(); }

		const TImpl*
		operator ->() const { return Get(); }

		TImpl&
		operator *() { return *Get(); }

		const TImpl&
		operator *() const { return *Get(); }

	private:
		template<typename>
		friend struct PImpl;

		std::unique_ptr<TImpl> m_impl;
	};

}