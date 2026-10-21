#include "MemberFunctionPointer.h"

namespace Oyl::Platform
{
	union MemberFunctionPointerRepresentation
	{
		void* functionPtr;

#	if defined(__OYL__ABI_MSC)
		struct
		{
			void* functionPtrOrThunk;
			int32_t nonVirtualBaseAdjustment;
			int32_t virtualBasePtrOffset;
			int32_t virtualBaseAdjustmentOffset;
		} msvc;
#	endif
#	if defined(__OYL__ABI_ITANIUM)
		struct
		{
			union
			{
				void* functionPtr;
				ptrdiff_t offset;
			};

			ptrdiff_t thisAdjustor;
		} itanium;
#	endif
	};

	void
	MemberFunctionPointerData::PopulateSizeof8(void* a_obj, void* a_memberFunctionPointer)
	{
		MemberFunctionPointerRepresentation mfp;
		std::memcpy(&mfp, a_memberFunctionPointer, 8);

		thisPtr = a_obj;

#	if defined(__OYL__ABI_ITANIUM)
		// Itanium ABI specifies that the first value is either:
		// - A pointer to the function
		// - An offset into the vtable in bytes, plus 1
		// Function addresses are guaranteed to have >1 byte alignment, so we can test against the 1s bit
		if (mfp.itanium.offset & 1)
		{
			using Fn = void();
			using VFnTable = Fn* const*;

			// thisPtr and virtual function table are the same address
			VFnTable vftable = *reinterpret_cast<VFnTable*>(thisPtr);
			size_t index = (mfp.itanium.offset - 1) / sizeof(Fn*); // offset is offset in bytes + 1

			// Index into the table to retrieve the function pointer
			thunkPtr = reinterpret_cast<void*>(vftable[index]);
		} else
#	endif
		// MSC ABI and Itanium non-virtual member functions point directly to the function, or a thunk function
		// Either way, calling it has the same effect as calling a member function through an interface
		{
			thunkPtr = mfp.functionPtr;
		}
	}

	void
	MemberFunctionPointerData::PopulateSizeof16(void* a_obj, void* a_memberFunctionPointer)
	{
		PopulateSizeof8(a_obj, a_memberFunctionPointer);

		MemberFunctionPointerRepresentation mfp;
		std::memcpy(&mfp, a_memberFunctionPointer, 16);

		// Adjust thisPtr by the specified amount in bytes
	#if defined(__OYL__ABI_MSC)
		static_assert(16 > offsetof(decltype(mfp.msvc), nonVirtualBaseAdjustment));
		reinterpret_cast<uintptr_t&>(thisPtr) += mfp.msvc.nonVirtualBaseAdjustment;
	#elif defined(__OYL__ABI_ITANIUM)
		static_assert(16 > offsetof(decltype(mfp.itanium), thisAdjustor));
		reinterpret_cast<uintptr_t&>(thisPtr) += mfp.itanium.thisAdjustor;
	#endif
	}

	void
	MemberFunctionPointerData::PopulateSizeof24(void* a_obj, void* a_memberFunctionPointer)
	{
		PopulateSizeof16(a_obj, a_memberFunctionPointer);

		MemberFunctionPointerRepresentation mfp;
		std::memcpy(&mfp, a_memberFunctionPointer, 24);

		// TODO: How do we handle virtual bases in MSC ABI?
	}
}
