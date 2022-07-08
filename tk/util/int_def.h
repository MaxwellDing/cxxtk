#pragma once

#include "stdint.h"

#if defined(WIN32) || defined(WINDOWS)
typedef unsigned __int64  u64_t;
typedef unsigned __int32  u32_t;
typedef unsigned __int16  u16_t;
typedef unsigned __int8   u8_t;
typedef __int64           i64_t;
typedef signed __int32    i32_t;
typedef signed __int16    i16_t;
typedef signed __int8     i8_t;
#else
typedef uint64_t          u64_t;
typedef uint32_t          u32_t;
typedef uint16_t          u16_t;
typedef uint8_t           u8_t;
typedef int64_t           i64_t;
typedef int32_t           i32_t;
typedef int16_t           i16_t;
typedef int8_t            i8_t;
#endif /*WIN32||WINDOWS*/

