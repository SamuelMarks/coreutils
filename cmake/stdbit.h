#ifndef _GL_STDBIT_H
#define _GL_STDBIT_H
#include <stdint.h>
static inline int stdc_trailing_zeros(unsigned int x) { return __builtin_ctz(x); }
static inline int stdc_leading_zeros(unsigned int x) { return __builtin_clz(x); }
static inline int stdc_leading_zeros_ull(unsigned long long x) { return __builtin_clzll(x); }
#endif
