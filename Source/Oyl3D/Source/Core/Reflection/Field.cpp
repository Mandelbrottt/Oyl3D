#include "Field.h"

#include "ReflectionFactory.h"
#include "Type.h"

namespace Oyl::Reflection
{
	Field::Field(const Internal::FieldParams& a_params)
		: Variable(a_params),
		  m_offset(static_cast<uint16>(a_params.offsetInBits / 8)),
		  m_offsetInBits(static_cast<uint16>(a_params.offsetInBits)),
		  m_isConst(a_params.isConst) {}

	bool
	Field::GetValueUnsafe(void* a_self, void** a_outPtr) const
	{
		uintptr_t valuePtr = reinterpret_cast<uintptr_t>(a_self);
		valuePtr += uintptr_t(m_offsetInBits / 8);
		*a_outPtr = reinterpret_cast<void*>(valuePtr);

		// TODO: Return false if field isn't valid
		return true;
	}
}
