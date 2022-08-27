#pragma once

#include "Type.h"
#include "TypeTraits.h"

#include "Core/Common.h"

// Reflection API inspired by https://www.educba.com/reflection-in-c-plus-plus/
#pragma region Reflection Macros
	#define _REFLECT_TYPE_ALIAS This
	#define _REFLECT_BASE_ALIAS Base

	#define _REFLECT_VALID_REFLECTION_TYPE_ALIAS _ConfirmValidReflectionType

	// msvc intrinsic offsetof doesn't work for types with v-tables
	#define _REFLECT_OFFSET_OF(_type_, _field_) ((size_t) &((_type_*) 0)->_field_)

	#define _REFLECT_NAMESPACE ::Rearm::Reflection::

	#define _REFLECT_CLASS_NAME _ReflectClassNameIdentifier

	#define _REFLECT_MEMBERS_ARGUMENT_NAME _r

	#define _REFLECT_DECLARE_COMMON(_class_) \
		_MACRO_AUTO_FORMAT_INDENT \
	private: \
		friend class _REFLECT_NAMESPACE Type; \
		typedef _class_ _REFLECT_TYPE_ALIAS; \
		static constexpr char* _REFLECT_CLASS_NAME = #_class_; \
		\
		static \
		void \
		_ReflectType(_REFLECT_NAMESPACE Type& r) \
		{ \
			if constexpr (_REFLECT_NAMESPACE is_complete_type_v<_REFLECT_BASE_ALIAS>) \
			{ \
				r._ReflectType<_REFLECT_BASE_ALIAS, _REFLECT_TYPE_ALIAS>(); \
			} \
		} \
		\
	public: \
		struct _REFLECT_VALID_REFLECTION_TYPE_ALIAS {}; \
		static \
		const _REFLECT_NAMESPACE Type& \
		GetStaticType() \
		{ \
			return _REFLECT_NAMESPACE Type::Get<_REFLECT_TYPE_ALIAS>(); \
		} \
		virtual \
		const _REFLECT_NAMESPACE Type& \
		GetType() \
		{ \
			return _REFLECT_NAMESPACE Type::Get<_REFLECT_TYPE_ALIAS>(); \
		} \
	private:\
		static \
		void \
		_ReflectMembers(_REFLECT_NAMESPACE Type& _REFLECT_MEMBERS_ARGUMENT_NAME)

	#define _REFLECT_DECLARE_1(_class_) \
		_MACRO_AUTO_FORMAT_INDENT \
	private: \
		struct _REFLECT_BASE_ALIAS; \
		class  _REFLECT_BASE_ALIAS; \
		_REFLECT_DECLARE_COMMON(_class_)

	#define _REFLECT_DECLARE_2(_class_, _base_) \
		_MACRO_AUTO_FORMAT_INDENT \
	private: \
		typedef _base_ _REFLECT_BASE_ALIAS; \
	public: \
		static \
		const _REFLECT_NAMESPACE Type& \
		GetBaseType() \
		{ \
			return _REFLECT_NAMESPACE Type::Get<_REFLECT_BASE_ALIAS>();\
		} \
		_REFLECT_DECLARE_COMMON(_class_)

	#define ReflectDeclare(...) _MACRO_OVERLOAD(_REFLECT_DECLARE, __VA_ARGS__)

	#pragma region Reflect Member Macros
		#define ReflectField(_field_, ...) \
			_REFLECT_MEMBERS_ARGUMENT_NAME._ReflectMember(_REFLECT_NAMESPACE Field {\
				/*.debugName        =*/ #_field_,\
				/*.fieldName        =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Name>(0, __VA_ARGS__, _REFLECT_NAMESPACE Name(#_field_)).c_str,\
				/*.fieldDescription =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Description>(0, __VA_ARGS__, _REFLECT_NAMESPACE Description("")).c_str,\
				/*.offset           =*/ (uint32_t) _REFLECT_OFFSET_OF(_REFLECT_TYPE_ALIAS, _field_), \
				/*.size             =*/ (uint32_t) sizeof(_field_), \
				/*.fieldType        =*/ _REFLECT_NAMESPACE GetTypeId<decltype(_field_)>(), \
				/*.containingType   =*/ _REFLECT_NAMESPACE GetTypeId<_REFLECT_TYPE_ALIAS>(),\
				/*.flags            =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE FieldFlags>(0, __VA_ARGS__, _REFLECT_NAMESPACE FieldFlags::None)\
			})

		#define ReflectFunction(_function_, ...) \
			_REFLECT_MEMBERS_ARGUMENT_NAME._ReflectMember(_REFLECT_NAMESPACE Function {\
				/*.debugName           =*/ #_function_,\
				/*.functionName        =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Name>(0, __VA_ARGS__, _REFLECT_NAMESPACE Name(#_function_)).c_str,\
				/*.functionDescription =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Description>(0, __VA_ARGS__, _REFLECT_NAMESPACE Description("")).c_str,\
				/*.function            =*/ [](void* a_instance) { reinterpret_cast<This*>(a_instance)->_function_(); }\
			})
	#pragma endregion

	// TODO: have sfinae struct to detect that only valid arguments are passed to reflect macros?

	// Possible ReflectMember general macro?
	/*
		if constexpr (std::is_member_object_pointer_v<decltype(&a)(This::*)>)
		{
			//Rearm::Reflection::Field* ptr;
			//_r._ReflectMember(*ptr);
		} else if constexpr (std::is_member_function_pointer_v<decltype(&a)>)
		{
			//Rearm::Reflection::Function* ptr;
			//_r._ReflectMember(*ptr);
		}
	 */
#pragma endregion