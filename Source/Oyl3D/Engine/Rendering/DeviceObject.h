#pragma once

namespace Oyl::Rendering::Internal
{
	class IDeviceObject
	{
	protected:
		IDeviceObject() = default;

		IDeviceObject(IDeviceObject&& a_other) noexcept = default;
		IDeviceObject&
		operator =(IDeviceObject&& a_other) noexcept = default;

	public:
		IDeviceObject(const IDeviceObject& a_other) = delete;
		IDeviceObject&
		operator =(const IDeviceObject& a_other) = delete;

		virtual
		~IDeviceObject() = default;

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
