#pragma once

#include "Profiler_Tracy.h"

#define OYL_PROFILER_ENABLED 1

#if OYL_PROFILER_ENABLED
#	define OYL_PROFILE_SCOPE(...)       OYL_MACRO_OVERLOAD(_OYL_PROFILE_SCOPE, __VA_ARGS__)
#	define _OYL_PROFILE_SCOPE_1(_name_) ZoneScopedN(_name_)

#	define OYL_PROFILE_FUNCTION() ZoneScoped

#	define OYL_FRAME_MARK()             FrameMark
#	define OYL_FRAME_MARK_NAMED(_name_) FrameMarkNamed

#	define OYL_FRAME_MARK_START(_name_) FrameMarkStart(_name_)
#	define OYL_FRAME_MARK_END(_name_)   FrameMarkEnd(_name_)
#else
#	define OYL_PROFILE_SCOPE(...) OYL_UNUSED(__VA_ARGS__)
#	define OYL_PROFILE_FUNCTION()
#	define OYL_FRAME_MARK()
#	define OYL_FRAME_MARK_NAMED(_name_) OYL_UNUSED(_name_)
#	define OYL_FRAME_MARK_START(_name_) OYL_UNUSED(_name_)
#	define OYL_FRAME_MARK_END(_name_)   OYL_UNUSED(_name_)
#endif
