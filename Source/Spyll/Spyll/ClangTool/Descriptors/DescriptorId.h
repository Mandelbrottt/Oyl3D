#pragma once

namespace Spyll
{
	enum class DescriptorId : uint32_t
	{
		Invalid = static_cast<std::underlying_type_t<DescriptorId>>(-1)
	};
}