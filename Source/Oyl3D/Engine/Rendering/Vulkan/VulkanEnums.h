#pragma once

#include <vulkan/vulkan.hpp>

#include <Core/EnumFlags.h>

#include "Rendering/Enums.h"

namespace Oyl::Rendering::Vulkan
{
	OYL_DEFINE_ENUM_FLAGS_CONVERSION_FUNCTIONS(
		ToOylEnum,
		ToVkEnum,
		CommandQueueFlags,
		vk::QueueFlags,
		CommandQueueFlagBits,
		vk::QueueFlagBits,
		{ CommandQueueFlagBits::Graphics, vk::QueueFlagBits::eGraphics },
		{ CommandQueueFlagBits::Compute, vk::QueueFlagBits::eCompute },
		{ CommandQueueFlagBits::Transfer, vk::QueueFlagBits::eTransfer },
	)

	OYL_DEFINE_ENUM_FLAGS_CONVERSION_FUNCTIONS(
		ToOylEnum,
		ToVkEnum,
		ImageUsageFlags,
		vk::ImageUsageFlags,
		ImageUsageFlagBits,
		vk::ImageUsageFlagBits,
		{ ImageUsageFlagBits::ColorAttachment, vk::ImageUsageFlagBits::eColorAttachment },
		{ ImageUsageFlagBits::DepthStencilAttachment, vk::ImageUsageFlagBits::eDepthStencilAttachment },
		{ ImageUsageFlagBits::TransferSrc, vk::ImageUsageFlagBits::eTransferSrc },
		{ ImageUsageFlagBits::TransferDst, vk::ImageUsageFlagBits::eTransferDst },
	)

	OYL_DEFINE_ENUM_FLAGS_CONVERSION_FUNCTIONS(
		ToOylEnum,
		ToVkEnum,
		PipelineStageFlags,
		vk::PipelineStageFlags,
		PipelineStageFlagBits,
		vk::PipelineStageFlagBits,
		{ PipelineStageFlagBits::ColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput },
	)

	OYL_DEFINE_ENUM_CONVERSION_FUNCTIONS(
		ToOylEnum,
		ToVkEnum,
		ImageFormat,
		vk::Format,
		{ ImageFormat::R8G8B8Srgb, vk::Format::eR8G8B8Srgb },
		{ ImageFormat::R8G8B8A8Srgb, vk::Format::eR8G8B8A8Srgb },
		{ ImageFormat::B8G8R8A8Srgb, vk::Format::eB8G8R8A8Srgb },
		{ ImageFormat::B8G8R8Srgb, vk::Format::eB8G8R8Srgb },
	)

	OYL_DEFINE_ENUM_CONVERSION_FUNCTIONS(
		ToOylEnum,
		ToVkEnum,
		ShaderProfile,
		vk::ShaderStageFlagBits,
		{ ShaderProfile::Vertex, vk::ShaderStageFlagBits::eVertex },
		{ ShaderProfile::Geometry, vk::ShaderStageFlagBits::eGeometry },
		{ ShaderProfile::Fragment, vk::ShaderStageFlagBits::eFragment },
	)

	OYL_DEFINE_ENUM_CONVERSION_FUNCTIONS(
		ToOylEnum,
		ToVkEnum,
		ImageLayout,
		vk::ImageLayout,
		{ ImageLayout::ColorAttachment, vk::ImageLayout::eColorAttachmentOptimal },
		{ ImageLayout::DepthStencilAttachment, vk::ImageLayout::eDepthStencilAttachmentOptimal },
		{ ImageLayout::ShaderRead, vk::ImageLayout::eShaderReadOnlyOptimal },
	)
}
