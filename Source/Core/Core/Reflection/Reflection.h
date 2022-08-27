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
#pragma endregion