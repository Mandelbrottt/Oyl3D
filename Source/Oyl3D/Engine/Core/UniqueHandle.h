#pragma once

namespace Oyl
{
	class IUniqueHandle
	{
	protected:
		IUniqueHandle() = default;

		DEFAULT_MOVE(IUniqueHandle);

	public:
		NO_COPY(IUniqueHandle);

		virtual
		~IUniqueHandle() = default;

		virtual
		void
		Destroy() = 0;

		virtual
		bool
		IsValid() const = 0;

		virtual
		explicit
		operator bool() const
		{
			return IsValid();
		}

		virtual
		bool
		operator !() const
		{
			return !IsValid();
		}
	};
}
