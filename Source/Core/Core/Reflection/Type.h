#pragma once

#include <map>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>

#include "Field.h"
#include "Function.h"
#include "Factory.h"
#include "TypeId.h"

#define DEPRECATED_WARNING_NUMBER 4996

#define HEAP_ALLOCATED_FACTORY
//#define IN_PLACE_FACTORY

namespace Rearm::Reflection
{
	/**
	 * \brief A runtime structure that represents a reflected data type.
	 *        Contains information about member fields, functions, and other type information. 
	 */
	class Type
	{
		using types_container_t = std::unordered_map<TypeId, Type>;

		static
		types_container_t
		s_typesContainer;

		explicit
		Type(const std::type_info& a_info) noexcept
			: m_typeInfo(&a_info)
		{
			ProcessName(a_info);
		}

		Type(const Type& a_other) = default;

		Type&
		operator =(const Type&) = delete;

	public:
		using instance_fields_container_t = std::vector<Field>;
		using member_function_container_t = std::vector<Function>;

		// Warning because move constructor is public?
		#pragma warning(disable : DEPRECATED_WARNING_NUMBER)
		Type(Type&& a_other) = default;

		Type&
		operator =(Type&&) = default;
		#pragma warning(disable : DEPRECATED_WARNING_NUMBER)

		bool
		operator ==(const Type& a_other) const
		{
			return m_typeId == a_other.m_typeId;
		}

		bool
		operator !=(const Type& a_other) const
		{
			return !(*this == a_other);
		}

		/**
		 * \brief Retrieve the statically assigned type id for the type represented by the current \link Type \endlink.
		 * \return The non-zero static type id if the current type is valid, zero otherwise.
		 */
		TypeId
		GetTypeId() const
		{
			return m_typeId;
		}

		/**
		 * \brief Retrieve the statically assigned type id for the base type of the type represented
		 *        by the current \link Type \endlink.
		 * \return The non-zero static type id if the current type is valid, zero otherwise.
		 */
		TypeId
		GetBaseTypeId() const
		{
			return m_baseTypeId;
		}

		/**
		 * \return The simplified name of the type represented by the current \link Type \endlink
		 */
		const std::string&
		Name() const
		{
			return m_name;
		}

		/**
		 * \return The fully-qualified name of the type represented by the current \link Type \endlink
		 */
		const std::string&
		FullName() const
		{
			return m_fullName;
		}

		const instance_fields_container_t&
		GetFields() const
		{
			return m_instanceFields;
		}

		const Field*
		GetField(std::string_view a_fieldName) const;

		const member_function_container_t&
		GetFunctions() const
		{
			return m_memberFunctions;
		}

		bool
		IsConvertibleTo(TypeId a_typeId) const;

		bool
		IsConvertibleTo(const Type* a_type) const;

		bool
		IsConvertibleTo(const Type& a_type) const;

		bool
		IsConvertibleFrom(TypeId a_typeId) const;

		bool
		IsConvertibleFrom(const Type* a_type) const;

		bool
		IsConvertibleFrom(const Type& a_type) const;

		/**
		 * \brief Retrieve the value returned by sizeof for the type class represented by
		 *        the current \link Type \endlink.
		 * \return The size of the type.
		 */
		int
		GetSize() const
		{
			return m_size;
		}

		const std::type_info&
		GetTypeInfo() const
		{
			return *m_typeInfo;
		}
		
		#pragma warning(disable : DEPRECATED_WARNING_NUMBER)
		// ReSharper disable once CppDeprecatedEntity
		/**
		 * \brief Get the instance of the \link GenericFactory \endlink associated with this type.
		 * \return A pointer to the \link GenericFactory \endlink associated with this type.
		 */
		const GenericFactory* GetFactory() const
		{
		#if defined HEAP_ALLOCATED_FACTORY
			return m_factory;
		#elif !defined IN_PLACE_FACTORY
			// See https://en.cppreference.com/w/cpp/types/aligned_storage
			auto pConstructor = std::launder(reinterpret_cast<GenericFactory const*>(m_factoryData));
			return pConstructor;
		#else
			return &m_factory;
		#endif
		}
		#pragma warning(disable : DEPRECATED_WARNING_NUMBER)

		/**
		 * \brief Register a type for reflection
		 * \tparam T The type to register for reflection.
		 *           Must contain reflection macros, see REFLECT_PREPARE and #REFLECT_MEMBERS
		 * \return An iterator for the container of Reflectors pointing to the newly registered element
		 */
		template<class T>
		static
		const Type&
		Register() noexcept;

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
		const Type&
		Get() noexcept;

		/**
		 * \brief Get a pointer to a \link Type \endlink representing T if it has already been registered.
		 * \param a_id The static id of the \link Type \endlink to get.
		 * \return A pointer to the \link Type \endlink for T if it has been registered, otherwise nullptr.
		 * \remarks Registering new elements can invalidate this pointer. If lazy-initializing Reflectors,
		 *          references and pointers should not be maintained beyond function scope. Lookup is O(1) time complexity,
		 *          so performance should not be a concern for retrieving the Reflector.
		 */
		static
		const Type*
		TryGet(TypeId a_id) noexcept;

		static
		const Type*
		TryGet(std::string_view const& a_fullyQualifiedTypeName);

		static
		const types_container_t&
		Enumerate()
		{
			return s_typesContainer;
		}

		// Used by REFLECT_MEMBERS
		template<typename TArg>
		void
		_ReflectMember(TArg&& a_arg) noexcept;

		// Used by REFLECT_DECLARE if reflecting the base class
		template<typename TBase, typename TDerived>
		inline
		void
		_ReflectType() noexcept;

	public:
	private:
		// We need this helper because you can't partially specialize functions to make sfinae work in
		// the way we want it to here, so do the reflection work in the static call function
		template<typename TReflected, typename = void>
		struct reflect_type_helper
		{
			static
			void
			call(Type& a_type) { }
		};
		
		template<typename TReflected>
		struct reflect_type_helper<TReflected, std::void_t<decltype(sizeof(&TReflected::_ReflectType))>>
		{
			static
			void
			call(Type& a_type) { TReflected::_ReflectType(a_type); }
		};
		
		template<typename TReflected, typename = void>
		struct reflect_members_helper
		{
			static
			void
			call(Type& a_type) { }
		};
		
		template<typename TReflected>
		struct reflect_members_helper<TReflected, std::void_t<decltype(sizeof(&TReflected::_ReflectMembers))>>
		{
			static
			void
			call(Type& a_type) { TReflected::_ReflectMembers(a_type); }
		};
		
		template<typename TReflected>
		static
		void
		PopulateFactory(Type& a_type);

		// Called by Type::Register
		template<class TReflected>
		static
		Type
		Reflect() noexcept;

		void
		ReflectInternal(Field&& a_fieldInfo) noexcept
		{
			m_instanceFields.emplace_back(std::move(a_fieldInfo));
		}
		
		void
		ReflectInternal(Function&& a_functionInfo) noexcept
		{
			m_memberFunctions.emplace_back(std::move(a_functionInfo));
		}

		/**
		 * \brief Convert the compiler-generated name from type_info.name() into the raw type name
		 */
		void ProcessName(std::type_info const& a_typeInfo);

	private:
		TypeId m_typeId = TypeId::Null;

		TypeId m_baseTypeId = TypeId::Null;

		std::type_info const* m_typeInfo;
		
		std::string m_name;

		std::string m_fullName;

		instance_fields_container_t m_instanceFields;
		member_function_container_t m_memberFunctions;

	#if defined HEAP_ALLOCATED_FACTORY
		GenericFactory* m_factory;
	#elif !defined IN_PLACE_FACTORY
		std::byte m_factoryData[sizeof(GenericFactory)] { std::byte() };
	#else
		/* TODO: Using placement new on stack-allocated type to manipulate the vtable
		         works in test scenarios but isn't working in practice. Figure out why */
		GenericFactory m_factory;
	#endif	
		//GenericConstructor m_constructor;

		int m_size = 0;
	};
}

#include "Type.inl"

// Write-Only
static Rearm::Reflection::Name r_name;

// Write-Only
static Rearm::Reflection::Description r_description;

// Write-Only
static Rearm::Reflection::FieldFlags r_flags;

#ifdef HEAP_ALLOCATED_FACTORY
	#undef HEAP_ALLOCATED_FACTORY
#endif

#ifdef IN_PLACE_FACTORY
	#undef IN_PLACE_FACTORY
#endif

#undef DEPRECATED_WARNING_NUMBER
