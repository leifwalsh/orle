// Copyright (c) 2012, Leif Walsh
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef RLE_TESTS_UNITTEST_H
#define RLE_TESTS_UNITTEST_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

// 99 says "epic failure" to automake
#define expect(cond) do {                                               \
        if (!(cond)) {                                                  \
            int errsv = errno;                                          \
            fprintf(stderr, "%s:%d: %s: Expectation failed: `" #cond "'\n", __FILE__, __LINE__, __func__); \
            errno = errsv;                                              \
            exit(99);                                                   \
        }                                                               \
    } while (0)

static bool test_is_passing = true;

#define assert(cond) do {                                               \
        if (!(cond)) {                                                  \
            int errsv = errno;                                          \
            fprintf(stderr, "%s:%d: %s: Assertion failed: `" #cond "'\n", __FILE__, __LINE__, __func__); \
            errno = errsv;                                              \
            test_is_passing = false;                                    \
        }                                                               \
    } while (0)

__attribute__((noreturn))
static void unittest_finish(void) {
    if (test_is_passing) {
        exit(EXIT_SUCCESS);
    } else {
        exit(EX_SOFTWARE);
    }
}

#endif // RLE_TESTS_UNITTEST_H
