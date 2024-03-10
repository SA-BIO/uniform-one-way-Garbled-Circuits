#include <iostream>
#include <chrono>
using namespace std::chrono;

#include "utils.h"
#include "haraka.h"



void print128(const u128& val) {
    byte temp[16];
    STORE_BYTES(temp, val);
    for (int i = 0; i < 16; i++) {
        std::cout << (int)temp[i] << ", ";
    }
    std::cout << std::endl;
}
void printBytes(byte valBytes[16]) {
    for (int i = 0; i < 16; i++) {
        std::cout << (int)valBytes[i] << ", ";
    }
    std::cout << std::endl;
}

void testVOLE(byte* u, u128 x, u128* v, u128* w) {
    int binErrors = 0;
    int nonMatchErrors = 0;
    for (int i = 0; i < EXPANSION_LENG; i++) {
        byte checker1[16];
        byte checker2[16];
        if (u[i] == 0) {
            STORE_BYTES(checker1, v[i]);
            STORE_BYTES(checker2, w[i]);

            for (int j = 0; j < 16; j++) {
                if (checker1[j] != checker2[j]) {
                    nonMatchErrors++;
                    break;
                }
            }
        }
        else if (u[i] == 1) {
            u128 aux = _mm_xor_si128(x, v[i]);
            STORE_BYTES(checker1, aux);
            STORE_BYTES(checker2, w[i]);

            for (int j = 0; j < 16; j++) {
                if (checker1[j] != checker2[j]) {
                    nonMatchErrors++;
                    break;
                }
            }
        }
        else {
            binErrors++;
        }
    }
    std::cout << "-------------------------------------------------------------------------" << "\n";
    std::cout << "BINARY ERRORS: " << binErrors << "\n";
    std::cout << ((binErrors == 0) ? "Vector u is awlays 0 or 1" : "Vector u is NOT awlays 0 or 1") << "\n";
    std::cout << "INCORRECT CORRELATION ERRORS: " << nonMatchErrors << "\n";
    std::cout << ((nonMatchErrors == 0) ? "All values are correctly correlated" : "Some values are NOT correctly correlated") << "\n";
    std::cout << "-------------------------------------------------------------------------" << std::endl;
}



static inline void rand128(u128& val) {
    byte randVals[16];
    for (int i = 0; i < 16; i++) {
        randVals[i] = rand();
    }
    val = LOAD_BYTES(randVals);
}

static inline void randBits(byte vals[16], int leng) {
    for (int i = 0; i < leng; i++) {
        vals[i] = rand() & 1;
    }
}

static inline void cMatUpdate(byte abcd[4]) {
    bool cond3 = abcd[3] == (START_LENG_BITS - 1);  //abcd[3] == 127
    bool cond2 = abcd[2] == (START_LENG_BITS - 2);  //abcd[2] == 126
    bool cond1 = abcd[1] == (START_LENG_BITS - 3);  //abcd[1] == 125

    abcd[0] += (cond3 & cond2 & cond1);
    abcd[1] = abcd[1^cond1] + (cond3&cond2);
    abcd[2] = abcd[((1^cond2)<<1)^cond2] + cond3;
    abcd[3] = abcd[3^cond3] + 1;
}


long long ot_expand_A(u128 x, u128 c[START_LENG_BITS], u128* w0, u128* w1) {
    auto start = high_resolution_clock::now();

    byte cMat[4] = { 0,1,2,2 };
    for (int i = 0; i < EXPANSION_LENG; i++) {
        cMatUpdate(cMat);
        w0[i] = _mm_xor_si128(_mm_xor_si128(c[cMat[0]], c[cMat[1]]), _mm_xor_si128(c[cMat[2]], c[cMat[3]]));
        w1[i] = _mm_xor_si128(w0[i], x);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    long long durationCast = duration.count();
    return duration.count();
}
long long ot_expand_B(byte a[START_LENG_BITS], u128 b[START_LENG_BITS], byte* u, u128* v) {
    auto start = high_resolution_clock::now();

    byte cMat[4] = { 0,1,2,2 };
    for (int i = 0; i < EXPANSION_LENG; i++) {
        cMatUpdate(cMat);
        u[i] = a[cMat[0]] ^ a[cMat[1]] ^ a[cMat[2]] ^ a[cMat[3]];
        v[i] = _mm_xor_si128(_mm_xor_si128(b[cMat[0]], b[cMat[1]]), _mm_xor_si128(b[cMat[2]], b[cMat[3]]));
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    long long durationCast = duration.count();
    return duration.count();
}

long long hash_example(u128* inpt1, u128* inpt2, u128* res1, u128* res2) {
    u128 harakaConsts[40];
    load_haraka_constants(harakaConsts);

    u128 randOT_aux_1;
    u128 randOT_aux_2;
    rand128(randOT_aux_1);
    rand128(randOT_aux_2);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < EXPANSION_LENG; i++) {
        haraka256_custom(inpt1[123], randOT_aux_1, res1[i], harakaConsts);
        haraka256_custom(inpt2[123], randOT_aux_2, res2[i], harakaConsts);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    long long durationCast = duration.count();
    return duration.count();
}


int main()
{
    constexpr byte zeroes[16]{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

    u128 x;
    byte a[START_LENG_BITS];
    u128 b[START_LENG_BITS];
    u128 c[START_LENG_BITS];

    //WARNING: this code emulates base OT generation (use proper base OT methods with good cryptographic randomness)
    rand128(x);
    u128 zeroX[2]{ LOAD_BYTES(zeroes), x };
    randBits(a, START_LENG_BITS);
    for (int i = 0; i < START_LENG_BITS; i++) {
        rand128(b[i]);
        c[i] = _mm_xor_si128(zeroX[a[i]], b[i]);
    }


    //OT EXTENSION--------------------------------------------------------------------------------------------
    byte* u = new byte[EXPANSION_LENG];
    u128* v = new u128[EXPANSION_LENG];
    u128* w0 = new u128[EXPANSION_LENG];
    u128* w1 = new u128[EXPANSION_LENG];

    long long timerA = ot_expand_A(x, c, w0, w1);
    long long timerB = ot_expand_B(a, b, u, v);
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "EXTENSION DURATION A: " << timerA << " milliseconds" << std::endl;
    std::cout << "EXTENSION DURATION B: " << timerB << " milliseconds" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    testVOLE(u, x, v, w0);
    //OT EXTENSION--------------------------------------------------------------------------------------------


    //HARAKA HASH---------------------------------------------------------------------------------------------
    u128* hTest_1 = new u128[EXPANSION_LENG];
    u128* hTest_2 = new u128[EXPANSION_LENG];
    long long timerH = hash_example(w0, w1, hTest_1, hTest_2);
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    std::cout << "HARAKA EXAMPLE DURATION: " << timerH << " milliseconds" << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    //HARAKA HASH---------------------------------------------------------------------------------------------

    delete[] u;
    delete[] v;
    delete[] w0;
    delete[] w1;
    delete[] hTest_1;
    delete[] hTest_2;

    std::cout << "END" << std::endl;
    char stopChar = std::getchar();
    std::cout << stopChar << std::endl;
}

