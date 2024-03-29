//HARAKA PAPER:   https://eprint.iacr.org/2016/098.pdf
//CODE FROM:      https://github.com/kste/haraka


#pragma once
#include "utils.h"


#define NUMROUNDS 5

typedef __m128i u128;

#define LOAD(src) _mm_load_si128((u128 *)(src))
#define STORE(dest,src) _mm_storeu_si128((u128 *)(dest),src)

#define AES2(s0, s1, rci) \
  s0 = _mm_aesenc_si128(s0, rc[rci]); \
  s1 = _mm_aesenc_si128(s1, rc[rci + 1]); \
  s0 = _mm_aesenc_si128(s0, rc[rci + 2]); \
  s1 = _mm_aesenc_si128(s1, rc[rci + 3]);

#define AES2_4x(s0, s1, s2, s3, rci) \
  AES2(s0[0], s0[1], rci); \
  AES2(s1[0], s1[1], rci); \
  AES2(s2[0], s2[1], rci); \
  AES2(s3[0], s3[1], rci);

#define AES2_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci) \
  AES2_4x(s0, s1, s2, s3, rci); \
  AES2_4x(s4, s5, s6, s7, rci);

#define AES4(s0, s1, s2, s3, rci) \
  s0 = _mm_aesenc_si128(s0, rc[rci]); \
  s1 = _mm_aesenc_si128(s1, rc[rci + 1]); \
  s2 = _mm_aesenc_si128(s2, rc[rci + 2]); \
  s3 = _mm_aesenc_si128(s3, rc[rci + 3]); \
  s0 = _mm_aesenc_si128(s0, rc[rci + 4]); \
  s1 = _mm_aesenc_si128(s1, rc[rci + 5]); \
  s2 = _mm_aesenc_si128(s2, rc[rci + 6]); \
  s3 = _mm_aesenc_si128(s3, rc[rci + 7]); \

#define AES4_4x(s0, s1, s2, s3, rci) \
  AES4(s0[0], s0[1], s0[2], s0[3], rci); \
  AES4(s1[0], s1[1], s1[2], s1[3], rci); \
  AES4(s2[0], s2[1], s2[2], s2[3], rci); \
  AES4(s3[0], s3[1], s3[2], s3[3], rci);

#define AES4_8x(s0, s1, s2, s3, s4, s5, s6, s7, rci) \
  AES4_4x(s0, s1, s2, s3, rci); \
  AES4_4x(s4, s5, s6, s7, rci);

#define MIX2(s0, s1) \
  tmp = _mm_unpacklo_epi32(s0, s1); \
  s1 = _mm_unpackhi_epi32(s0, s1); \
  s0 = tmp;

#define MIX4(s0, s1, s2, s3) \
  tmp  = _mm_unpacklo_epi32(s0, s1); \
  s0 = _mm_unpackhi_epi32(s0, s1); \
  s1 = _mm_unpacklo_epi32(s2, s3); \
  s2 = _mm_unpackhi_epi32(s2, s3); \
  s3 = _mm_unpacklo_epi32(s0, s2); \
  s0 = _mm_unpackhi_epi32(s0, s2); \
  s2 = _mm_unpackhi_epi32(s1, tmp); \
  s1 = _mm_unpacklo_epi32(s1, tmp);

#define TRUNCSTORE(out, s0, s1, s2, s3) \
  *(u64*)(out) = (u64*)(s0)[1]; \
  *(u64*)(out + 8) = (u64*)(s1)[1]; \
  *(u64*)(out + 16) = (u64*)(s2)[0]; \
  *(u64*)(out + 24) = (u64*)(s3)[0];

void load_haraka_constants(u128 rc[40]);

//CUSTOM IMPLEMENTATION OF HARAKA FOR THIS PROJECT
inline void haraka256_custom(u128& in_1, u128& in_2, u128& res, u128 rc[40]) {
	u128 s[2], tmp;

	s[0] = in_1;
	s[1] = in_2;

	AES2(s[0], s[1], 0);
	MIX2(s[0], s[1]);

	AES2(s[0], s[1], 4);
	MIX2(s[0], s[1]);

	AES2(s[0], s[1], 8);
	MIX2(s[0], s[1]);

	AES2(s[0], s[1], 12);
	MIX2(s[0], s[1]);

	AES2(s[0], s[1], 16);
	MIX2(s[0], s[1]);

	res = _mm_xor_si128(s[0], in_1);
	//s[0] = _mm_xor_si128(s[0], in_1);
	//s[1] = _mm_xor_si128(s[1], in_2);
	
	//STORE(out, s[0]);
	//STORE(out + 16, s[1]);
}
