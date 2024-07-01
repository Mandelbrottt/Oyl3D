#pragma once

#include "Core/Types/TypeId.h"

namespace Oyl::Reflection::Detail
{
	// Surround typed_factory_constructible with helper struct that client classes can make a friend
	// Normally, template instantiations don't have the same friend semantics as the class they are attached to,
	// so private members cannot be checked against. If they are surrounded by a struct that is a friend, however,
	// private members can be checked again
	struct typed_factory_constructible_helper
	{
		template<typename T, typename = void>
		struct typed_factory_constructible : std::false_type {};

		template<typename T>
		struct typed_factory_constructible<T, std::void_t<decltype(T {})>> : std::true_type {};
	};

	template<typename T>
	using typed_factory_constructible = typed_factory_constructible_helper::typed_factory_constructible<T>;

	template<typename T>
	constexpr static bool typed_factory_constructible_v = typed_factory_constructible<T>::value;

	/**
	 * \brief Wrapper around construction and destruction of objects, statically evaluated
	 *        with placement new and delete through polymorphism
	 * \remark All base functions return nullptr or don't do anything instead of being pure virtual
	 *         because then we wouldn't be able to construct this on the stack.
	 */
	class GenericFactory
	{
	public:
		using size_type = uint64;

		virtual
		~GenericFactory() = default;

		virtual
		TypeId
		GetTypeId() const
		{
			return TypeId::Null;
		}

		virtual
		bool
		Valid() const
		{
			return false;
		}

		virtual
		size_type
		Size() const
		{
			return 0;
		}

		virtual
		void*
		New() const
		{
			return nullptr;
		}

		virtual
		void*
		New(void* a_location) const
		{
			return nullptr;
		};

		virtual
		void
		Delete(void* a_location, bool a_deallocate = true) const { }
	};

	template<typename T, bool = typed_factory_constructible_v<T>>
	class TypedFactory;

	/**
	 * \brief A factory for creating objects that do not have default constructors, or
	 *        for classes that have non-public constructors and do not declare the TypedFactory as a friend.
	 * \tparam T The underlying type to construct
	 */
	template<typename T>
	class TypedFactory<T, false> final : public GenericFactory
	{
		static_assert(Detail::typed_factory_constructible_v<T> == false);

	public:
		using value_type = T;

		TypedFactory() = default;

		TypeId
		GetTypeId() const override
		{
			return Oyl::GetTypeId<T>();
		}

		size_type
		Size() const override
		{
			return sizeof(T);
		}
	};

	/**
	 * \brief A factory for creating objects that either have default constructors, or
	 *        for classes that declare the TypedFactory as a friend. Mainly useful for components.
	 * \tparam T The underlying type to construct
	 */
	template<typename T>
	class TypedFactory<T, true> final : public GenericFactory
	{
		static_assert(Detail::typed_factory_constructible_v<T> == true);

	public:
		using value_type = T;

		TypeId
		GetTypeId() const override
		{
			return Oyl::GetTypeId<T>();
		}

		bool
		Valid() const override
		{
			return true;
		}

		size_type
		Size() const override
		{
			return sizeof(T);
		}

		/**
		 * \brief Allocates a new object and returns it
		 * \return A pointer to the constructed object
		 */
		void*
		New() const override
		{
			return new value_type;
		}

		/**
		 * \brief Construct an object of type value_type in-place.
		 * \param a_location A pointer to the location to construct the object.
		 * \return The location where the object was constructed.
		 * \remark It is the caller's responsibility to ensure a_location is at least sizeof(value_type) bytes large.
		 *     <br>Currently returns the same value passed in.
		 */
		void*
		New(void* a_location) const override
		{
			return new(a_location) value_type;
		}

		/**
		 * \brief Destruct and deallocate the object at a_location.
		 * \param a_location A pointer to the object to destruct.
		 * \param a_deallocate Whether to deallocate the object.
		 * \remark If a_deallocate is false, or the object was constructed using
		 *         in-place \link New \endlink, deallocation is the caller's responsibility.
		 */
		void
		Delete(void* a_location, bool a_deallocate = true) const override
		{
			value_type* p = static_cast<value_type*>(a_location);

			if (a_deallocate)
			{
				delete p;
			} else
			{
				p->~value_type();
			}
		}
	};
}

