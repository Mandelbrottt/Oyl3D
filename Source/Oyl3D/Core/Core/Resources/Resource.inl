#pragma once

#include "Resource.h"

namespace Oyl
{
	template<typename TResource>
	constexpr ResourceTypeId
	Resource<TResource>::GetResourceTypeId()
	{
		static_assert(std::is_convertible_v<TResource*, Resource*>);
		return Reflection::GetTypeId<TResource>();
	}

	template<typename TResource>
	bool
	DeviceResource<TResource>::Load()
	{
		SetDeviceDirty();
		return Resource<TResource>::Load();
	}

	template<typename TResource>
	bool
	DeviceResource<TResource>::Unload()
	{
		SetDeviceDirty();
		return Resource<TResource>::Unload();
	}

	template<typename TResource>
	void
	DeviceResource<TResource>::SetDirty()
	{
		SetDeviceDirty();
		Resource<TResource>::SetDirty();
	}

	template<typename TResource>
	bool
	DeviceResource<TResource>::DeviceLoad(void* /*a_params*/)
	{
		m_deviceLoaded = true;
		m_deviceDirty = false;
		return m_deviceLoaded;
	}

	template<typename TResource>
	bool
	DeviceResource<TResource>::DeviceUnload(void* /*a_params*/)
	{
		m_deviceLoaded = false;
		m_deviceDirty = true;
		return m_deviceLoaded;
	}
}
