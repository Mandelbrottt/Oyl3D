#pragma once

#include "oylpch.h"
#include "Common.h"

#include <glad/glad.h>

#if defined(OYL_DEBUG)
#define GLCall(x) { while (glGetError() != GL_NO_ERROR); x; _GLLogCall(#x, __FILE__, __LINE__); }
#else
#define GLCall(x) x
#endif

static void _GLLogCall(const char* function, const char* file, int line) {
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		LOG_ERROR("\a{0} in {1} : {2}", function, file, line);
		//BREAKPOINT;
	}
}
