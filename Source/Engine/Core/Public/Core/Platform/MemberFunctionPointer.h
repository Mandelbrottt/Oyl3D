#pragma once

#include "Core/Preprocessor/Common.h"
#include "Core/Types/TypeTraits.h"

namespace Oyl::Platform
{
	class MemberFunctionPointerData
	{
		MemberFunctionPointerData() = default;

	public:
		template<typename... TArgs, typename TObj, typename TMemberFunctionPointer>
			requires requires(TObj a_obj, TMemberFunctionPointer a_memberFunctionPointer, TArgs&&... a_args)
			{
				{ (a_obj.*a_memberFunctionPointer)(std::forward<TArgs>(a_args)...) };
			}
		static
		MemberFunctionPointerData
		Create(TObj* a_obj, TMemberFunctionPointer a_pMemberFunction)
		{
			MemberFunctionPointerData result;
			void* pBytes = reinterpret_cast<void*>(&a_pMemberFunction);
			if constexpr (sizeof(a_pMemberFunction) == 8)
				result.PopulateSizeof8(a_obj, pBytes);
			else if constexpr (sizeof(a_pMemberFunction) == 16)
				result.PopulateSizeof16(a_obj, pBytes);
			else if constexpr (sizeof(a_pMemberFunction) == 24)
				result.PopulateSizeof24(a_obj, pBytes);
			return result;
		}

		void* thisPtr = nullptr;
		void* thunkPtr = nullptr;

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