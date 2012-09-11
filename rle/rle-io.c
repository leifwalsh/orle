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
#include <stdio.h>

#include "config.h"
#include "rle.h"
#include "rle-debug.h"
#include "rle-io.h"

int rle_encode_file(FILE * const dst, FILE * const src) {
    assert(dst);
    assert(src);
    return ENOTSUP;
}

int rle_decode_file(FILE * const dst, FILE * const src) {
    assert(dst);
    assert(src);
    return ENOTSUP;
}

__attribute__((warn_unused_result, nonnull(3,4)))
static int fdopen_dst_src(int dst, int src, FILE ** const dstfp, FILE ** const srcfp) {
    int r;
    FILE *dstf = fdopen(dst, "a");
    if (!dstf) {
        r = errno;
        goto exit;
    }
    FILE *srcf = fdopen(src, "r");
    if (!srcf) {
        r = errno;
        int cr = fclose(dstf);
        if (cr && rle_debug) {
            perror("fdopen_dst_src: failed to fclose dstf, can't express this error");
        }
        goto exit;
    }
    *dstfp = dstf;
    *srcfp = srcf;
    r = 0;
exit:
    return r;
}

__attribute__((warn_unused_result, nonnull(1,2)))
static int fclose_dst_src(FILE ** const dstf, FILE ** const srcf) {
    int r, r1, r2, errsv1, errsv2;
    r1 = fclose(*dstf);
    if (r1) {
        errsv1 = errno;
    } else {
        *dstf = NULL;
    }
    r2 = fclose(*srcf);
    if (r2) {
        errsv2 = errno;
    } else {
        *srcf = NULL;
    }
    if (r1 && r2) {
        if (rle_debug) {
            errno = errsv2;
            perror("fclose_dst_src: failed to close srcf, can't express this error");
        }
        errno = errsv1;
        r = r1;
    } else if (r1) {
        r = r1;
        errno = errsv1;
    } else if (r2) {
        r = r2;
        errno = errsv2;
    } else {
        r = 0;
    }
    return r;
}

int rle_encode_fd(int dst, int src) {
    int r, cr;
    FILE *dstf, *srcf;
    r = fdopen_dst_src(dst, src, &dstf, &srcf);
    if (r) {
        goto exit;
    }

    r = rle_encode_file(dstf, srcf);

    cr = fclose_dst_src(&dstf, &srcf);
    if (!r && cr) {
        r = cr;
    }
exit:
    return r;
}

int rle_decode_fd(int dst, int src) {
    int r, cr;
    FILE *dstf, *srcf;
    r = fdopen_dst_src(dst, src, &dstf, &srcf);
    if (r) {
        goto exit;
    }

    r = rle_decode_file(dstf, srcf);

    cr = fclose_dst_src(&dstf, &srcf);
    if (!r && cr) {
        r = cr;
    }
exit:
    return r;
}
