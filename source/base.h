/*===============================================================================
  @File:   base.h
  @Brief:  
  @Author: Tejas
  @Date:   28-08-2025
  @Notice: Released under the MIT License. See LICENSE file for details.
  ===============================================================================*/

#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Logging macro: only enabled in debug builds
#ifndef NDEBUG
#  define LOG(...) printf(__VA_ARGS__)
#else
#  define LOG(...) ((void)0)
#endif

// Figuring out the Operating System
#if defined(_WIN32)
#  define ON_WINDOWS 1
#elif defined(__linux__)
#  define ON_LINUX 1
#endif

// Figuring out the Compiler
#if defined(__clang__)
#  define CLANG_COMPILER 1
#elif defined(_MSC_VER)
#  define CL_COMPILER 1
#endif

// Resetting the rest of the Macros to 0
#if !defined(ON_WINDOWS)
#  define ON_WINDOWS 0
#endif
#if !defined(ON_LINUX)
#  define ON_LINUX 0
#endif
#if !defined(CLANG_COMPILER)
#  define CLANG_COMPILER 0
#endif
#if !defined(CL_COMPILER)
#  define CL_COMPILER 0
#endif

#define KB(x)   ((unsigned long long)(x) * 1024ULL)
#define MB(x)   (KB(x) * 1024ULL)
#define GB(x)   (MB(x) * 1024ULL)

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(*(x)))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define SUCCESS(x) ((x) ? false : true)

typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#define global   static 
#define persist  static 
#define internal static 

#endif // BASE_H
