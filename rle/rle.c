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

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "rle.h"
#include "rle-debug.h"

int rle_encode_bytes(unsigned char * const dst, size_t * const dstlen,
                     unsigned char const * const src, size_t srclen) {
    DBG("enter\n");
    assert(dstlen);
    assert(*dstlen >= srclen);

    int r;
    size_t srcidx = 0;
    size_t dstidx = 0;
    while (srcidx < srclen) {
        int8_t rl = 1;
        while (srcidx + rl < srclen && src[srcidx + rl] == src[srcidx] && rl < INT8_MAX) {
            ++rl;
        }
        if (dstidx + 2 >= *dstlen) {
            DBG("need at least %zu bytes in dst to continue\n", dstidx + 2);
            r = ENOBUFS;
            goto cleanup;
        }
        assert(rl > 0);
        dst[dstidx++] = rl;
        dst[dstidx++] = src[srcidx];
        srcidx += rl;
    }
    *dstlen = dstidx;
    r = 0;

cleanup:
    DBG("return %d\n", r);
    return r;
}

int rle_decode_bytes(unsigned char * const dst, size_t * const dstlen,
                     unsigned char const * const src, size_t srclen) {
    DBG("enter\n");
    assert(dstlen);
    assert(*dstlen >= srclen);

    int r;

    size_t srcidx = 0;
    size_t dstidx = 0;
    while (srcidx < srclen) {
        assert(srclen - srcidx >= 2);
        int8_t rl = src[srcidx++];
        for (int8_t i = 0; i < rl; ++i) {
            if (dstidx >= *dstlen) {
                DBG("need at least %zu bytes in dst to continue\n", dstidx + 1);
                r = ENOBUFS;
                goto cleanup;
            }
            dst[dstidx++] = src[srcidx];
        }
        ++srcidx;
    }

    *dstlen = srclen;
    r = 0;

cleanup:
    DBG("return %d\n", r);
    return r;
}
