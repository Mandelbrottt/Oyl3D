#pragma once

#include "Type.h"

#include "Core/Common.h"

#pragma region Reflection Macros
// Compiler intrinsic offsetof doesn't work on virtual classes, so roll our own to bypass this
#	define _REFLECT_OFFSET_OF(_type_, _field_) ((size_t) &((_type_*) 0)->_field_)

#	define _REFLECT_NAMESPACE ::Oyl::Reflection::

#	define _REFLECT_CLASS_NAME _ReflectClassNameIdentifier_
#	define _REFLECT_THIS_TYPE_ALIAS This
#	define _REFLECT_BASE_TYPE_ALIAS Base
#	define _REFLECT_VALID_REFLECTION_TYPE_ALIAS _ConfirmValidReflectionType_

#	define _REFLECT_DECLARE_COMMON(_class_) \
		OYL_FORCE_FORMAT_INDENT \
	private: \
		typedef _class_ _REFLECT_THIS_TYPE_ALIAS; \
		friend class _REFLECT_NAMESPACE Type; \
		static constexpr char* _REFLECT_CLASS_NAME = #_class_; \
		\
		static \
		void \
		_Reflect_(_REFLECT_NAMESPACE Type* r) \
		{ \
			/*if constexpr (_REFLECT_NAMESPACE is_complete_type_v<_REFLECT_BASE_TYPE_ALIAS>) \
			{ \
				r.operator()<_REFLECT_BASE_TYPE_ALIAS, _REFLECT_THIS_TYPE_ALIAS>(); \
			}*/ \
		} \
		static \
		void \
		_ReflectMembers_(_REFLECT_NAMESPACE Type* a_type); \
		\
	public: \
		struct _REFLECT_VALID_REFLECTION_TYPE_ALIAS {}; \
		static \
		_REFLECT_NAMESPACE Type* \
		GetStaticType() \
		{ \
			return _REFLECT_NAMESPACE Type::Get<_REFLECT_THIS_TYPE_ALIAS>();\
		} \
		virtual \
		_REFLECT_NAMESPACE Type* \
		GetType() \
		{ \
			return _REFLECT_NAMESPACE Type::Get<_REFLECT_THIS_TYPE_ALIAS>();\
		}

#	define _REFLECT_DECLARE_1(_class_) \
		OYL_FORCE_FORMAT_INDENT \
	private: \
		struct _REFLECT_BASE_TYPE_ALIAS; \
		class  _REFLECT_BASE_TYPE_ALIAS; \
		_REFLECT_DECLARE_COMMON(_class_)

#	define _REFLECT_DECLARE_2(_class_, _base_) \
		_REFLECT_DECLARE_COMMON(_class_) \
	private: \
		typedef _base_ _REFLECT_BASE_TYPE_ALIAS; \
	public: \
		static \
		_REFLECT_NAMESPACE Type* \
		GetBaseType() \
		{ \
			return _REFLECT_NAMESPACE Type::Get<_REFLECT_BASE_TYPE_ALIAS>();\
		}

#	define ReflectDeclare(...) OYL_MACRO_OVERLOAD(_REFLECT_DECLARE, __VA_ARGS__)

#	define ReflectMembers(_class_, _list_) \
		void \
		_class_::_ReflectMembers_(_REFLECT_NAMESPACE Type* a_type) \
		{ \
			using _REFLECT_THIS_TYPE_ALIAS = _class_; \
			a_type _list_; \
		} \
		OYL_FORCE_SEMICOLON

#	pragma region Reflect Field Overloads
		//#define _REFLECT_FIELD_PROTOTYPE(_field_, ...) \
		//	(_REFLECT_NAMESPACE Field {\
		//		/*.debugName        =*/ #_field_,\
		//		/*.fieldName        =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Name>(__VA_ARGS__, _REFLECT_NAMESPACE Name(#_field_)).c_str,\
		//		/*.fieldDescription =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE Description>(__VA_ARGS__, _REFLECT_NAMESPACE Description("")).c_str,\
		//		/*.offset           =*/ (uint32_t) _REFLECT_OFFSET_OF(_REFLECT_TYPE_ALIAS, _field_), \
		//		/*.size             =*/ (uint32_t) sizeof(_field_), \
		//		/*.fieldType        =*/ _REFLECT_NAMESPACE GetTypeId<decltype(_field_)>(), \
		//		/*.containingType   =*/ _REFLECT_NAMESPACE GetTypeId<_REFLECT_TYPE_ALIAS>(),\
		//		/*.flags            =*/ _REFLECT_NAMESPACE Pick<_REFLECT_NAMESPACE FieldFlags>(__VA_ARGS__, _REFLECT_NAMESPACE FieldFlags::None)\
		//	})
#		define _REFLECT_FIELD_PROTOTYPE(_field_, ...) \
			->_Reflect_(_REFLECT_NAMESPACE Field { \
				/*.debugName        =*/ #_field_, \
				/*.fieldName        =*/ #_field_, \
				/*.fieldDescription =*/ "", \
				/*.offset           =*/ (::Oyl::uint32) _OYL_EXPAND(_REFLECT_OFFSET_OF(_REFLECT_THIS_TYPE_ALIAS, _field_)), \
				/*.size             =*/ (::Oyl::uint32) sizeof(_REFLECT_THIS_TYPE_ALIAS::_field_), \
				/*.fieldType        =*/ ::Oyl::GetTypeId<decltype(_REFLECT_THIS_TYPE_ALIAS::_field_)>(), \
				/*.containingType   =*/ ::Oyl::GetTypeId<_REFLECT_THIS_TYPE_ALIAS>(), \
				/*.flags            =*/ _REFLECT_NAMESPACE FieldFlags::None \
			})

#		define _REFLECT_FIELD_1(_field_) _REFLECT_FIELD_2(_field_, #_field_)

#		define _REFLECT_FIELD_2(_field_, _arg1_) \
			_REFLECT_FIELD_PROTOTYPE(_field_, _arg1_)

#		define _REFLECT_FIELD_3(_field_, _arg1_, _arg2_) \
			_REFLECT_FIELD_PROTOTYPE(_field_, _arg1_, _arg2_)

#		define _REFLECT_FIELD_4(_field_, _arg1_, _arg2_, _arg3_) \
			_REFLECT_FIELD_PROTOTYPE(_field_, _arg1_, _arg2_, _arg3_)

#		define _REFLECT_FIELD_5(_field_, _arg1_, _arg2_, _arg3_, _arg4_) \
			_REFLECT_FIELD_PROTOTYPE(_field_, _arg1_, _arg2_, _arg3_, _arg4_)

#		define _REFLECT_FIELD_6(_field_, _arg1_, _arg2_, _arg3_, _arg4_, _arg5_) \
			_REFLECT_FIELD_PROTOTYPE(_field_, _arg1_, _arg2_, _arg3_, _arg4_, _arg5_)

#		define _REFLECT_FIELD_7(_field_, _arg1_, _arg2_, _arg3_, _arg4_, _arg5_, _arg6_) \
			_REFLECT_FIELD_PROTOTYPE(_field_, _arg1_, _arg2_, _arg3_, _arg4_, _arg5_, _arg6_)

#		define _REFLECT_FIELD_8(_field_, _arg1_, _arg2_, _arg3_, _arg4_, _arg5_, _arg6_, _arg7_) \
			_REFLECT_FIELD_PROTOTYPE(_field_, _arg1_, _arg2_, _arg3_, _arg4_, _arg5_, _arg6_, _arg7_)

#		define ReflectField(...) OYL_MACRO_OVERLOAD(_REFLECT_FIELD, __VA_ARGS__)
