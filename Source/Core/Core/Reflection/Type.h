#pragma once

#pragma once

#include <typeinfo>
#include <type_traits>
#include <unordered_map>

#include "Field.h"
#include "TypedFactory.h"
#include "Core/Types/TypeId.h"
#include "Core/Types/Typedefs.h"

//#define HEAP_ALLOCATED_FACTORY

namespace Oyl::Reflection
{
	/**
	 * \brief A runtime structure that represents a reflected data type.
	 *        Contains information about member fields, functions, and other type information. 
	 */
	class OYL_CORE_API Type
	{
		using TypesContainer = std::unordered_map<TypeId, Type>;

		static TypesContainer& Types()
		{
			static TypesContainer container {};
			return container;
		}

		explicit
		Type(std::type_info const& a_info) noexcept
			: m_typeInfo(&a_info)
		{
			ProcessName(a_info);
		}

		Type(Type&& a_other) = default;
		Type&
		operator =(Type&&) = default;

	public:
		using InstanceFieldsContainer = std::vector<Field>;

		Type(Type const& a_other) = default;

		~Type();

		Type&
		operator =(Type const&) = delete;

		bool
		operator ==(Type const& a_other) const
		{
			return m_typeId == a_other.m_typeId;
		}

		bool
		operator !=(Type const& a_other) const
		{
			return !(*this == a_other);
		}

		/**
		 * \brief Retrieve the statically assigned type id for the type represented by the current \link Type \endlink.
		 * \return The non-zero static type id if the current type is valid, zero otherwise.
		 */
		Oyl::TypeId
		TypeId() const
		{
			return m_typeId;
		}

		/**
		 * \brief Retrieve the statically assigned type id for the base type of the type represented
		 *        by the current \link Type \endlink.
		 * \return The non-zero static type id if the current type is valid, zero otherwise.
		 */
		Oyl::TypeId
		BaseTypeId() const
		{
			return m_baseTypeId;
		}

		/**
		 * \brief Retrieve the value returned by sizeof for the type class represented by
		 *        the current \link Type \endlink.
		 * \return The size of the type.
		 */
		int
		Size() const
		{
			return m_size;
		}

		std::type_info const&
		TypeInfo() const
		{
			return *m_typeInfo;
		}

		/**
		 * \return The simplified name of the type represented by the current \link Type \endlink
		 */
		std::string const&
		Name() const
		{
			return m_name;
		}

		/**
		 * \return The fully-qualified name of the type represented by the current \link Type \endlink
		 */
		std::string const&
		FullName() const
		{
			return m_fullName;
		}

		InstanceFieldsContainer const&
		InstanceFields() const
		{
			return m_instanceFields;
		}

		bool
		IsConvertibleTo(Oyl::TypeId a_typeId) const;

		bool
		IsConvertibleTo(Type const* a_type) const;

		bool
		IsConvertibleTo(Type const& a_type) const;

		bool
		IsConvertibleFrom(Oyl::TypeId a_typeId) const;

		bool
		IsConvertibleFrom(Type const* a_type) const;

		bool
		IsConvertibleFrom(Type const& a_type) const;

		/**
		 * \brief Get the instance of the \link Detail::GenericFactory \endlink associated with this type.
		 * \return A pointer to the \link Detail::GenericFactory \endlink associated with this type.
		 */
		const Detail::GenericFactory* Factory() const
		{
		#if defined HEAP_ALLOCATED_FACTORY
			return m_factory;
		#else !defined IN_PLACE_FACTORY
			// See https://en.cppreference.com/w/cpp/types/aligned_storage
			auto pFactory = std::launder(reinterpret_cast<const Detail::GenericFactory*>(m_factoryData));
			return pFactory;
		#endif
		}

		/**
		 * \brief Get a lazy-initialized \link Type \endlink representing T.
		 * \tparam T The type to retrieve the \link Type \endlink for.
		 * \return A \link Type \endlink for the given type.
		 * \remarks Registering new elements can invalidate this reference. If lazy-initializing Reflectors,
		 *          references and pointers should not be maintained beyond function scope. Lookup is O(1) time complexity,
		 *          so performance should not be a concern for retrieving the Reflector.
		 */
		template<class T>
		static
		Type&
		Get() noexcept
		{
			Oyl::TypeId id = GetTypeId<T>();

			auto iter = Types().find(id);
			if (iter == Types().end())
			{
				iter = Register<T>();
			}
			return iter->second;
		}

		/**
		 * \brief Get a pointer to a \link Type \endlink representing T if it has already been registered.
		 * \param a_id The static id of the \link Type \endlink to get.
		 * \return A pointer to the \link Type \endlink for T if it has been registered, otherwise nullptr.
		 * \remarks Registering new elements can invalidate this pointer. If lazy-initializing Reflectors,
		 *          references and pointers should not be maintained beyond function scope. Lookup is O(1) time complexity,
		 *          so performance should not be a concern for retrieving the Reflector.
		 */
		static
		Type*
		TryGet(Oyl::TypeId a_id) noexcept;

		static
		Type*
		TryGet(std::string_view a_fullyQualifiedTypeName);

		static
		TypesContainer&
		Enumerate()
		{
			return Types();
		}

		/**
		 * \brief Register a type for reflection
		 * \tparam T The type to register for reflection.
		 *           Must contain reflection macros, see REFLECT_PREPARE and REFLECT_MEMBERS
		 * \return An iterator for the container of Reflectors pointing to the newly registered element
		 */
		template<class T>
		static
		TypesContainer::iterator
		Register() noexcept
		{
			Oyl::TypeId id = GetTypeId<T>();

			auto& types = Types();

			if (auto iter = types.find(id); iter != types.end())
			{
				return iter;
			}

			Type type             = Type::Reflect<T>();
			auto iter             = types.emplace(id, std::move(type)).first;
			iter->second.m_typeId = id;
			iter->second.m_size   = sizeof(T);

			return iter;
		};

		// Used by REFLECT_MEMBERS
		inline
		Type&
		_Reflect_(Field&& a_field) noexcept
		{
			ReflectInternal(std::move(a_field));

			return *this;
		}

		// Used by REFLECT_DECLARE if reflecting the base class
		template<typename TBase, typename TDerived>
		inline
		Type&
		_Reflect_() noexcept
		{
			static_assert(std::is_base_of_v<TBase, TDerived>,
				"Second argument of REFLECT_DECLARE MUST be derived from the first!");

			// Don't need to use TryGet here because this function can only be called if there is a base class
			auto& baseType = Get<TBase>();

			m_baseTypeId = baseType.GetTypeId();

			if (!baseType.m_instanceFields.empty())
			{
				auto& baseFields = baseType.m_instanceFields;

				m_instanceFields.insert(m_instanceFields.begin(), baseFields.begin(), baseFields.end());
			}

			return *this;
		}

	private:
		// You can't partially specialize function templates, so ReflectType has to be a struct-wrapped function
		template<typename TReflected, typename = void>
		struct ReflectType
		{
			static
			void
			Call(Type* a_type)
			{
				OYL_UNUSED(a_type);
			}
		};

		template<typename TReflected>
		struct ReflectType<TReflected, std::void_t<decltype(&TReflected::_Reflect_)>>
		{
			static
			void
			Call(Type* a_type)
			{
				TReflected::_Reflect_(a_type);
			}
		};

		template<typename TReflected>
		static
		void
		PopulateFactory(Type* a_type)
		{
			// Heap Allocated Factory supersedes in place factory
#		if defined HEAP_ALLOCATED_FACTORY
			// Use pointer and new to allow free passing around of the factory without fear of
			// Reallocation
			a_type.m_factory = new Detail::TypedFactory<TReflected>;
#		else
			// Generate the constructor/destructor and use placement new under the hood
			// We use vtable shenanigans to make this work, so the data represented
			// by reflector.m_factoryData is stored "on the stack" inside this type while
			// being able to reference it like a pointer and use the vtable to call overridden
			// versions of New and Delete
			auto pFactory = reinterpret_cast<Detail::GenericFactory*>(a_type->m_factoryData);
			static_assert(sizeof(*pFactory) == sizeof(Detail::TypedFactory<TReflected>));

			// ReSharper disable once CppDeprecatedEntity
			new(pFactory) Detail::TypedFactory<TReflected>;
#		endif
		}

		// Called by Type::Register
		template<class TReflected>
		static
		Type
		Reflect() noexcept
		{
			Type reflector(typeid(TReflected));

			ReflectType<TReflected>::Call(&reflector);

			auto caseInsensitivePredicate = [](Field const& a_lhs, Field const& a_rhs)
			{
				auto const& lhsName = a_lhs.displayName;
				auto const& rhsName = a_rhs.displayName;

				return _strcmpi(lhsName.c_str(), rhsName.c_str()) < 0;
			};

			auto& fields = reflector.m_instanceFields;
			std::sort(fields.begin(), fields.end(), caseInsensitivePredicate);

			PopulateFactory<TReflected>(&reflector);

			return reflector;
		}

		void
		ReflectInternal(Field&& a_fieldInfo) noexcept
		{
			m_instanceFields.emplace_back(std::move(a_fieldInfo));
		}

		/**
		 * \brief Convert the compiler-generated name from type_info.name() into the raw type name
		 */
		void
		ProcessName(std::type_info const& a_typeInfo);

	private:
		Oyl::TypeId m_typeId = TypeId::Null;

		Oyl::TypeId m_baseTypeId = TypeId::Null;

		std::type_info const* m_typeInfo;

		std::string m_name;

		std::string m_fullName;

		InstanceFieldsContainer m_instanceFields;

	#if defined HEAP_ALLOCATED_FACTORY
		Detail::GenericFactory* m_factory;
	#else
		alignas(Detail::GenericFactory)
		byte m_factoryData[sizeof(Detail::GenericFactory)] { byte() };
	#endif

		int m_size = 0;
	};
}

#ifdef HEAP_ALLOCATED_FACTORY
	#undef HEAP_ALLOCATED_FACTORY
#endif

#undef DEPRECATED_WARNING_NUMBER
