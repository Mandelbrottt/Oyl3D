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

		template<typename TPointeeOther>
			requires Traits::ConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>
		UniquePtr(UniquePtr<TPointeeOther>&& a_other) noexcept
			: m_impl(std::unique_ptr<TPointee>(static_cast<Traits::TAddPointer<TPointee>>(a_other.m_impl.release()))) {}

		template<typename TPointeeOther>
			requires (Traits::ExplicitlyConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>
			          && !Traits::ImplicitlyConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>)
		explicit
		UniquePtr(UniquePtr<TPointeeOther>&& a_other) noexcept
			: m_impl(std::unique_ptr<TPointee>(static_cast<Traits::TAddPointer<TPointee>>(a_other.m_impl.release()))) {}

		template<typename TPointeeOther>
			requires Traits::ConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>
		UniquePtr&
		operator =(UniquePtr<TPointeeOther>&& a_other) noexcept
		{
			m_impl = std::unique_ptr<TPointee>(static_cast<Traits::TAddPointer<TPointee>>(a_other.m_impl.release()));
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

		explicit
		UniquePtrImplicitConvertible(Super&& a_other) noexcept
			: Super(std::move(a_other)) {}

		template<typename TPointeeOther>
			requires Traits::ConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>
		UniquePtrImplicitConvertible(UniquePtr<TPointeeOther>&& a_other) noexcept
			: Super(std::move(a_other)) {}

		template<typename TPointeeOther>
			requires (Traits::ExplicitlyConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>
			          && !Traits::ImplicitlyConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>)
		explicit
		UniquePtrImplicitConvertible(UniquePtr<TPointeeOther>&& a_other) noexcept
			: Super(std::move(a_other)) {}

		template<typename TPointeeOther>
			requires Traits::ConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>
		UniquePtrImplicitConvertible(UniquePtrImplicitConvertible<TPointeeOther>&& a_other) noexcept
			: Super(std::move(a_other)) {}

		template<typename TPointeeOther>
			requires (Traits::ExplicitlyConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>
			          && !Traits::ImplicitlyConvertibleTo<Traits::TAddPointer<TPointeeOther>, Traits::TAddPointer<TPointee>>)
		explicit
		UniquePtrImplicitConvertible(UniquePtrImplicitConvertible<TPointeeOther>&& a_other) noexcept
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
