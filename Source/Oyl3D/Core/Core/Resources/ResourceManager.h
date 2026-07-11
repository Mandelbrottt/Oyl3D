#pragma once

#include "Core/Resources/ResourceHandle.h"

namespace Oyl::Internal
{
	class ResourceManager final
	{
		friend ResourceHandleBase;

	public:
		ResourceManager();

		~ResourceManager();

		ResourceHandleBase
		CreateHandle(ResourceTypeId a_typeId);

		void
		DestroyHandle(ResourceHandleBase& a_handle);

	private:
		Resource*
		GetResource(ResourceTypeId a_typeId, ResourceId a_id);

		struct Impl;
		Impl* m_impl;
	};
}
