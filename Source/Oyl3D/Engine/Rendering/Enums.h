#pragma once

#include "Core/EnumFlags.h"

namespace Oyl::Rendering
{
	//////////////////////////////////////////////////////////////////////////////

	enum class CommandQueueFlagBits : uint32
	{
		Graphics = 1 << 0,
		Compute = 1 << 1,
		Transfer = 1 << 2,
	};

	OYL_ENUM_CLASS_BITWISE_OPERATIONS(CommandQueueFlagBits)

	using CommandQueueFlags = EnumFlags<CommandQueueFlagBits>;

	//////////////////////////////////////////////////////////////////////////////

	enum class ImageUsageFlagBits : uint32
	{
		ColorAttachment = 1 << 0,
		DepthStencilAttachment = 1 << 1,
		TransferSrc = 1 << 2,
		TransferDst = 1 << 3,
	};

	OYL_ENUM_CLASS_BITWISE_OPERATIONS(ImageUsageFlagBits)

	using ImageUsageFlags = EnumFlags<ImageUsageFlagBits>;

	//////////////////////////////////////////////////////////////////////////////

	enum class PipelineStageFlagBits : uint32
	{
		ColorAttachmentOutput = 1 << 0,
	};

	OYL_ENUM_CLASS_BITWISE_OPERATIONS(PipelineStageFlagBits)

	using PipelineStageFlags = EnumFlags<PipelineStageFlagBits>;

	//////////////////////////////////////////////////////////////////////////////

	enum class ImageFormat
	{
		None,

		R8G8B8Srgb,
		R8G8B8A8Srgb,

		B8G8R8Srgb,
		B8G8R8A8Srgb
	};

	enum class ImageLayout
	{
		None,

		ColorAttachment,
		DepthStencilAttachment,
		ShaderRead
	};

	//////////////////////////////////////////////////////////////////////////////

	enum class ShaderLanguage
	{
		None,

		Hlsl,
	};

	//////////////////////////////////////////////////////////////////////////////

	enum class ShaderProfile
	{
		None,

		Vertex,
		Geometry,
		Fragment,
	};
}
