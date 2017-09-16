#pragma once
#define attr(...) __attribute__((__VA_ARGS__))
#define inline_always inline attr(always_inline)
