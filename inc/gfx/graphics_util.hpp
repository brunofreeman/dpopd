#ifndef __GRAPHICS_UTIL_HPP__
#define __GRAPHICS_UTIL_HPP__

#define gl_catch_error(x) gl_clear_error(); x; gl_log_call(#x, __FILE__, __LINE__)

bool gl_log_call(const char* function, const char* file, int line);

void gl_clear_error();

void gl_print_version();

#endif  // __GRAPHICS_UTIL_HPP__