#pragma once

#define LFTAB "\n\t"
#define DECL_SECTION(sec) __attribute__((section(#sec)))
#define DECL_NOINLINE __attribute__((noinline))
#define DECL_PACKED __attribute__((packed))
#define SECTOR_MEM  0x7C00
#define SECTOR_SIZE 512
#define STACK_SIZE  4096

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <assert.h>
