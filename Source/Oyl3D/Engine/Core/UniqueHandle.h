#pragma once

namespace Oyl
{
	class IUniqueHandle
	{
	protected:
		IUniqueHandle() = default;

		IUniqueHandle(IUniqueHandle&& a_other) noexcept = default;
		IUniqueHandle&
		operator =(IUniqueHandle&& a_other) noexcept = default;

	public:
		IUniqueHandle(const IUniqueHandle& a_other) = delete;
		IUniqueHandle&
		operator =(const IUniqueHandle& a_other) = delete;

		virtual
		~IUniqueHandle() = default;

		virtual
		void
		Destroy() = 0;

		virtual
		bool
		IsValid() const = 0;

		explicit
		operator bool() const
		{
			return IsValid();
		}
	};
}
