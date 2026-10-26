#include "ImGuiImpl.h"

namespace Oyl::Internal
{
	ImGuiContext* g_context;
	ImGuiMemAllocFunc g_allocFunc;
	ImGuiMemFreeFunc g_freeFunc;
	void* g_userData;

	ImGuiContext*
	ImGuiGetCurrentContext()
	{
		return g_context;
	}

	void
	ImGuiSetCurrentContext(ImGuiContext* a_context)
	{
		g_context = a_context;
	}

	void
	ImGuiGetAllocatorFunctions(ImGuiMemAllocFunc* a_allocFunc, ImGuiMemFreeFunc* a_freeFunc, void** a_userData)
	{
		*a_allocFunc = g_allocFunc;
		*a_freeFunc = g_freeFunc;
		*a_userData = g_userData;
	}

	void
	ImGuiSetAllocatorFunctions(ImGuiMemAllocFunc a_allocFunc, ImGuiMemFreeFunc a_freeFunc, void* a_userData)
	{
		g_allocFunc = a_allocFunc;
		g_freeFunc = a_freeFunc;
		g_userData = a_userData;
	}
}
