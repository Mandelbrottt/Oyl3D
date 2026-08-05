#pragma once

#include "Core/TypeTraits.h"

namespace Oyl
{
	template<typename TPointee>
	class UniquePtr
	{
	public:
		UniquePtr() noexcept
			: m_impl(nullptr) {}

		UniquePtr(std::nullptr_t) noexcept
			: UniquePtr() {}

		DEFAULT_MOVE(UniquePtr);

		explicit
		UniquePtr(std::unique_ptr<TPointee>&& a_impl) noexcept
			: m_impl(std::move(a_impl)) {}

		template<typename TPointeeChild>
			requires std::is_convertible_v<std::add_pointer_t<TPointeeChild>, std::add_pointer_t<TPointee>>
		UniquePtr(UniquePtr<TPointeeChild>&& a_other) noexcept
			: m_impl(std::move(a_other.m_impl)) {}

		template<typename TPointeeChild>
			requires std::is_convertible_v<std::add_pointer_t<TPointeeChild>, std::add_pointer_t<TPointee>>
		UniquePtr&
		operator =(UniquePtr<TPointeeChild>&& a_other) noexcept
		{
			std::swap(m_impl, a_other.m_impl);
			return *this;
		}

		template<typename... TArgs>
		static
		UniquePtr
		Create(TArgs... a_args)
		{
			return UniquePtr(
				std::make_unique<TPointee>(std::forward<TArgs>(a_args)...)
			);
		}

		explicit
		operator bool() const noexcept
		{
			return !!m_impl;
		}

		TPointee*
		Get() noexcept { return m_impl.get(); }

		const TPointee*
		Get() const noexcept { return m_impl.get(); }

		TPointee*
		operator ->() { return Get(); }

		const TPointee*
		operator ->() const { return Get(); }

		TPointee&
		operator *() { return *Get(); }

		const TPointee&
		operator *() const { return *Get(); }

		TPointee*
		operator &() noexcept { return Get(); }

		const TPointee*
		operator &() const noexcept { return Get(); }

		void
		Reset(TPointee* a_pointee = nullptr) noexcept
		{
			m_impl.reset(a_pointee);
		}

		TPointee*
		Release() noexcept
		{
			return m_impl.release();
		}

	private:
		template<typename>
		friend class UniquePtr;

		std::unique_ptr<TPointee> m_impl;
	};

	template<typename TPointee>
	class UniquePtrImplicitConvertible : public UniquePtr<TPointee>
	{
		using Super = UniquePtr<TPointee>;

	public:
		UniquePtrImplicitConvertible() noexcept
			: Super() {}

		UniquePtrImplicitConvertible(std::nullptr_t) noexcept
			: UniquePtrImplicitConvertible() {}

		DEFAULT_MOVE(UniquePtrImplicitConvertible);

		explicit
		UniquePtrImplicitConvertible(std::unique_ptr<TPointee>&& a_impl) noexcept
			: Super(std::move(a_impl)) {}

		template<typename TPointeeChild>
			requires std::is_pointer_interconvertible_base_of_v<TPointeeChild, TPointee>
		UniquePtrImplicitConvertible(UniquePtr<TPointeeChild>&& a_other) noexcept
			: Super(std::move(a_other)) {}

		operator TPointee&()
		{
			return Super::operator *();
		}

		operator const TPointee&() const
		{
			return Super::operator *();
		}
	};
}
