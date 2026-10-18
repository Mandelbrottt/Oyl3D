#pragma once

namespace Oyl
{
	class IDestroyable
	{
	public:
		virtual
		~IDestroyable() = 0;

		virtual
		void
		Destroy() = 0;

		virtual
		bool
		IsValid() const = 0;
	};

	inline
	IDestroyable::~IDestroyable() {}
}
