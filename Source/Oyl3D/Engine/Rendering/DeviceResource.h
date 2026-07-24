#pragma once

#include "Core/Resources/Resource.h"

#include "Rendering/Device.h"

namespace Oyl::Rendering
{
	template<typename TResource>
	class DeviceResource : public Resource<TResource>
	{
	protected:
		DeviceResource() {}

	public:
		bool
		IsDeviceLoaded() const
		{
			return m_deviceLoaded;
		}

		bool
		IsDeviceDirty() const
		{
			return m_deviceDirty;
		}

		bool
		Load() override
		{
			SetDeviceDirty();
			return Resource<TResource>::Load();
		}

		bool
		Unload() override
		{
			SetDeviceDirty();
			return Resource<TResource>::Unload();
		}

	protected:
		void
		SetDirty() override
		{
			SetDeviceDirty();
			Resource<TResource>::SetDirty();
		}

		void
		SetDeviceDirty()
		{
			m_deviceDirty = true;
		}

		virtual
		bool
		DeviceLoad(const Device& a_device)
		{
			m_deviceLoaded = true;
			m_deviceDirty = false;
			return m_deviceLoaded;
		}

		virtual
		bool
		DeviceUnload()
		{
			m_deviceLoaded = false;
			m_deviceDirty = true;
			return m_deviceLoaded;
		}

	private:
		bool m_deviceLoaded = false;
		bool m_deviceDirty = true;
	};
}
