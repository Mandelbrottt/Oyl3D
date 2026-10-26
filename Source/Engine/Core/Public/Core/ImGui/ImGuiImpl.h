#pragma once

#include <ImGui/imgui.h>

namespace Oyl::Internal
{
	CORE_EXPORT
	ImGuiContext*
	ImGuiGetCurrentContext();

	CORE_EXPORT
	void
	ImGuiSetCurrentContext(ImGuiContext* a_context);

	CORE_EXPORT
	void
	ImGuiGetAllocatorFunctions(ImGuiMemAllocFunc* a_allocFunc, ImGuiMemFreeFunc* a_freeFunc, void** a_userData);

	CORE_EXPORT
	void
	ImGuiSetAllocatorFunctions(ImGuiMemAllocFunc a_allocFunc, ImGuiMemFreeFunc a_freeFunc, void* a_userData);
}