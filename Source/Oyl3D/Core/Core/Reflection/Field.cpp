#include "Field.h"

#include "Type.h"

namespace Spyll::Reflection
{
	bool
	Field::GetValue(void* a_self, void** a_outPtr) const
	{
		uintptr_t valuePtr = reinterpret_cast<uintptr_t>(a_self);
		valuePtr += uintptr_t(m_offset);
		*a_outPtr = reinterpret_cast<void*>(valuePtr);

		// TODO: Return false if field isn't valid
		return true;
	}
}
