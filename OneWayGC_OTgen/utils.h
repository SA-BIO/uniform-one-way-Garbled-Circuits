#pragma once
#include "immintrin.h"

typedef __m128i u128;
typedef unsigned long u64;
typedef unsigned char byte;

#define LOAD_BYTES(src) _mm_load_si128((u128 *)(src))
#define STORE_BYTES(dest,src) _mm_storeu_si128((u128 *)(dest),src)

#define START_LENG_BITS 128
#define EXPANSION_LENG 10000000