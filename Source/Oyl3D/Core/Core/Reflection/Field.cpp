#include "Field.h"

#include "ReflectionFactory.h"
#include "Type.h"

namespace Oyl::Reflection
{
	Field::Field(const Internal::FieldParams& a_params)
		: Variable(a_params),
		  MemberInfo(a_params),
		  m_offsetInBits(a_params.offsetInBits) {}

	bool
	Field::GetValue(void* a_self, void** a_outPtr) const
	{
		uintptr_t valuePtr = reinterpret_cast<uintptr_t>(a_self);
		valuePtr += uintptr_t(m_offsetInBits / 8);
		*a_outPtr = reinterpret_cast<void*>(valuePtr);

		// TODO: Return false if field isn't valid
		return true;
	}
}
