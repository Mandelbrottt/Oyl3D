#pragma once

#include "Core/Preprocessor/Common.h"
#include "Core/Types/TypeTraits.h"

namespace Oyl::Platform
{
	struct MemberFunctionPointerData
	{
#	define MEMBER_FUNCTION_POINTER_DATA_CONSTRUCTOR(_cv_ref_noexcept_) \
		template<typename TObj, typename TReturn, typename TClass, typename... TArgs> \
		MemberFunctionPointerData(TObj* a_obj, TReturn (TClass::*a_pMemberFunction)(TArgs...) _cv_ref_noexcept_) \
		{ \
			TClass* obj = static_cast<TClass*>(a_obj); \
			void* pBytes = reinterpret_cast<void*>(&a_pMemberFunction); \
			if constexpr (sizeof(a_pMemberFunction) == 8) \
				PopulateSizeof8(obj, pBytes); \
			else if constexpr (sizeof(a_pMemberFunction) == 16) \
				PopulateSizeof16(obj, pBytes); \
			else if constexpr (sizeof(a_pMemberFunction) == 24) \
				PopulateSizeof24(obj, pBytes); \
		}
		_CLASS_DEFINE_CV_REF_NOEXCEPT(MEMBER_FUNCTION_POINTER_DATA_CONSTRUCTOR)
#	undef MEMBER_FUNCTION_POINTER_DATA_CONSTRUCTOR

		void* thisPtr;
		void* thunkPtr;

	private:
		CORE_EXPORT
		void
		PopulateSizeof8(void* a_obj, void* a_memberFunctionPointer);

		CORE_EXPORT
		void
		PopulateSizeof16(void* a_obj, void* a_memberFunctionPointer);

		CORE_EXPORT
		void
		PopulateSizeof24(void* a_obj, void* a_memberFunctionPointer);
	};
}