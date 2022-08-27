#pragma once

namespace Rearm::Reflection
{
	namespace Detail
	{
		// Normally, you can't take sizeof void, so this meta function replaces sizeof
		// in cases where you may need to take the size of void (usually in template functions)
		template<typename T>
		struct sizeof_void_tolerant;
		
		template<>
		struct sizeof_void_tolerant<void>
		{
			constexpr static size_t value = 0;
		};
		
		template<typename T>
		struct sizeof_void_tolerant
		{
			constexpr static size_t value = sizeof(T);
		};

		template<typename T>
		constexpr size_t sizeof_void_tolerant_v = sizeof_void_tolerant<T>::value;
	}
	
	template<class T>
	const Type&
	Type::Register() noexcept
	{
		TypeId id = Reflection::GetTypeId<T>();

		auto& types = s_typesContainer;
		auto  iter  = types.find(id);

		if (iter != types.end())
		{
			return iter->second;
		}

		Type type             = Type::Reflect<T>();
		iter                  = types.emplace(id, std::move(type)).first;
		iter->second.m_typeId = id;
		iter->second.m_size   = Detail::sizeof_void_tolerant_v<T>;

		return iter->second;
	}

	template<class T>
	const Type&
	Type::Get() noexcept
	{
		TypeId id = Reflection::GetTypeId<T>();

		auto iter = s_typesContainer.find(id);
		if (iter == s_typesContainer.end())
		{
			auto& type = Register<T>();
			return type;
		}
		return iter->second;
	}

	template<typename TArg>
	void
	Type::_ReflectMember(TArg&& a_arg) noexcept
	{
		ReflectInternal(std::move(a_arg));
	}

	template<typename TBase, typename TDerived>
	void
	Type::_ReflectType() noexcept
	{
		static_assert(
			std::is_base_of_v<TBase, TDerived>,
			"Second argument of REFLECT_DECLARE MUST be derived from the first!"
		);

		// Don't need to use TryGet here because this function can only be called if there is a base class
		auto& baseType = Get<TBase>();

		m_baseTypeId = baseType.GetTypeId();

		if (!baseType.m_instanceFields.empty())
		{
			auto& baseFields = baseType.m_instanceFields;

			m_instanceFields.insert(m_instanceFields.begin(), baseFields.begin(), baseFields.end());
		}
	}

	template<typename TReflected>
	void
	Type::PopulateFactory(Type& a_type)
	{
	#pragma warning(disable : DEPRECATED_WARNING_NUMBER)

		// Heap Allocated Factory supersedes in place factory
	#if defined HEAP_ALLOCATED_FACTORY
		// Use pointer and new to allow free passing around of the factory without fear of
		// Reallocation
		a_type.m_factory = new TypedFactory<TReflected>;
	#else
		#if !defined IN_PLACE_FACTORY
			// Generate the constructor/destructor and use placement new under the hood
			// We use vtable shenanigans to make this work, so the data represented
			// by reflector.m_constructorData is stored "on the stack" inside this type while
			// being able to reference it like a pointer and use the vtable to call overridden
			// versions of Construct and Destruct
			auto pConstructor = reinterpret_cast<GenericFactory*>(a_type.m_factoryData);
		#else
			GenericFactory* pConstructor = &a_type.m_factory;
		#endif
			static_assert(sizeof(*pConstructor) == sizeof(TypedFactory<TReflected>));
			
			// ReSharper disable once CppDeprecatedEntity
			new(pConstructor) TypedFactory<TReflected>;
	#endif

	#pragma warning(disable : DEPRECATED_WARNING_NUMBER)
	}

	template<class TReflected>
	Type
	Type::Reflect() noexcept
	{
		Type reflector(typeid(TReflected));

		reflect_type_helper<TReflected>::call(reflector);
		reflect_members_helper<TReflected>::call(reflector);

		auto caseInsensitivePredicate = [](const Field& a_lhs, const Field& a_rhs)
		{
			const auto& lhsName = a_lhs.DisplayName();
			const auto& rhsName = a_rhs.DisplayName();

			return _strcmpi(lhsName.c_str(), rhsName.c_str()) < 0;
		};

		auto& fields = reflector.m_instanceFields;

		std::sort(fields.begin(), fields.end(), caseInsensitivePredicate);

		PopulateFactory<TReflected>(reflector);

		return reflector;
	}
}
