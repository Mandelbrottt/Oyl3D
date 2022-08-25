#pragma once

#include "TypeId.h"

namespace Rearm::Reflection
{
	/**
	 * \brief Wrapper around construction and destruction of objects, statically evaluated
	 *        with placement new and delete through polymorphism
	 * \remark All base functions return nullptr or don't do anything instead of being pure virtual
	 *         because then we wouldn't be able to construct this on the stack.
	 */
	struct GenericFactory
	{
		bool valid = false;

		uint32_t size = 0;

		virtual
		~GenericFactory()
		{
			valid = false;
			size  = 0;
		}

		virtual
		TypeId
		GetTypeId() const
		{
			return TypeId::Null;
		}

		virtual
		void*
		Construct() const
		{
			return nullptr;
		}

		virtual
		void*
		Construct(void* a_location) const
		{
			return nullptr;
		};

		virtual
		void
		Destruct(void* a_location, bool a_deallocate = true) const { }
	};

	namespace Detail
	{
		// Break normal sfinae convention here
		// Both values have to be in the same struct since types will friend the struct, and we can't
		// check if we can construct T in the struct's template parameters because the parameters
		// would technically be outside the struct
		struct typed_factory_friend_helper
		{
			template<typename T, typename = void>
			constexpr static bool value = false;

			template<typename T>
			constexpr static bool value<T, std::void_t<decltype(sizeof(T {}))>> = true;
		};

		template<typename T>
		constexpr static bool typed_factory_friend_helper_v = typed_factory_friend_helper::value<T>;
	}

	template<typename T, bool = Detail::typed_factory_friend_helper_v<T>>
	struct TypedFactory;

	/**
	 * \brief A factory for creating objects that do not have default constructors, or
	 *        for classes that do not declare the TypedFactory as a friend.
	 * \tparam T The underlying type to construct
	 */
	template<typename T>
	struct TypedFactory<T, false> final : GenericFactory
	{
		static_assert(Detail::typed_factory_friend_helper_v<T> == false);

		using value_type = T;

		constexpr static int value_size = sizeof(T);

		TypedFactory()
		{
			valid = true;
			size  = value_size;
		}

		TypeId
		GetTypeId() const override
		{
			return Reflection::GetTypeId<T>();
		}
	};
	
	/**
	 * \brief A factory for creating objects that either have default constructors, or
	 *        for classes that declare the TypedFactory as a friend. Mainly useful for components.
	 * \tparam T The underlying type to construct
	 */
	template<typename T>
	struct TypedFactory<T, true> final : GenericFactory
	{
		static_assert(Detail::typed_factory_friend_helper_v<T> == true);

		using value_type = T;

		constexpr static int value_size = sizeof(T);

		TypedFactory()
		{
			// valid is only set to true if GenericConstructor has been overridden
			valid = true;
			size  = value_size;
		}

		~TypedFactory() override = default;

		TypeId
		GetTypeId() const override
		{
			return Reflection::GetTypeId<T>();
		}

		/**
		 * \brief Allocates a new object and returns it
		 * \return A pointer to the constructed object
		 */
		void*
		Construct() const override
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
		Construct(void* a_location) const override
		{
			return new(a_location) value_type;
		}

		/**
		 * \brief Destruct and deallocate the object at a_location.
		 * \param a_location A pointer to the object to destruct.
		 * \param a_deallocate Whether to deallocate the object.
		 * \remark If a_deallocate is false, or the object was constructed using
		 *         in-place \link Construct \endlink, deallocation is the caller's responsibility.
		 */
		void
		Destruct(void* a_location, bool a_deallocate = true) const override
		{
			// Call the destructor
			value_type* p = static_cast<value_type*>(a_location);
			p->~value_type();

			// De-allocate the object
			if (a_deallocate)
			{
				::operator delete(a_location);
			}
		}
	};
}
