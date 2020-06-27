#ifndef __GFX_UTIL_HPP__
#define __GFX_UTIL_HPP__

#define ASSERT(x) if (!(x)) __debugbreak();
#define GL_CALL(x) gl_clear_error();\
    x;\
    ASSERT(gl_log_call(#x, __FILE__, __LINE__))

bool gl_log_call(const char* function, const char* file, int line);

void gl_clear_error();

void gl_print_version();

#endif  // __GFX_UTIL_HPP__