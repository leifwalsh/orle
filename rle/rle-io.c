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
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "rle.h"
#include "rle-debug.h"
#include "rle-io.h"

int rle_encode_file(FILE * const dst, FILE * const src) {
    DBG("enter\n");
    assert(dst);
    assert(src);

    int r;
    const size_t pagesize = sysconf(_SC_PAGESIZE);
    const size_t encbuflen = rle_encode_size_bound(pagesize);
    {
        unsigned char decbuf[pagesize];
        unsigned char encbuf[encbuflen];
        size_t readlen;
        while ((readlen = fread(decbuf, 1, pagesize, src)) > 0) {
            if (readlen < pagesize) {
                if ((r = ferror(src))) {
                    errno = r;
                    perror("rle_encode_file: fread");
                    goto exit;
                }
            }
            size_t enclen = rle_encode_size_bound(readlen);
            size_t encoded = rle_encode_bytes(encbuf, &enclen, decbuf, readlen);
            if (encoded < readlen) {
                perror("rle_encode_file: rle_encode_bytes");
                r = -1;
                goto exit;
            }
            size_t writelen = fwrite(encbuf, 1, enclen, dst);
            if (writelen < enclen) {
                r = ferror(dst);
                assert(r);
                errno = r;
                perror("rle_encode_file: fwrite");
                goto exit;
            }
        }
        assert(readlen == 0);
        assert(feof(src));
        r = 0;
    }

exit:
    DBG("return %d\n", r);
    return r;
}

int rle_decode_file(FILE * const dst, FILE * const src) {
    DBG("enter\n");
    assert(dst);
    assert(src);

    int r;
    const size_t pagesize = sysconf(_SC_PAGESIZE);
    {
        unsigned char encbuf[pagesize];
        unsigned char decbuf[pagesize];
        size_t readlen;
        size_t leftover = 0;
        while ((readlen = fread(&encbuf[leftover], 1, pagesize - leftover, src)) > 0) {
            if (readlen < pagesize - leftover) {
                if ((r = ferror(src))) {
                    errno = r;
                    perror("rle_decode_file: fread");
                    goto exit;
                }
            }
            size_t enclen = readlen + leftover;
            leftover = 0;
            DBG("decoding chunk of %zu...\n", enclen);
            size_t decoded_total = 0;
            while (decoded_total < enclen) {
                DBG("decoded %zu/%zu\n", decoded_total, enclen);
                size_t decoded;
                size_t declen = pagesize;
                decoded = rle_decode_bytes(decbuf, &declen,
                                           &encbuf[decoded_total], enclen - decoded_total);
                assert(declen <= pagesize);
                if (decoded < enclen - decoded_total) {
                    if (errno != ENOBUFS) {
                        perror("rle_decode_file: rle_decode_bytes");
                        r = -1;
                        goto exit;
                    }
                }
                if (decoded == 0) {
                    memmove(&encbuf[0], &encbuf[decoded_total], enclen - decoded_total);
                    leftover = enclen - decoded_total;
                    break;
                }

                size_t writelen = fwrite(decbuf, 1, declen, dst);
                if (writelen < declen) {
                    r = ferror(dst);
                    assert(r);
                    errno = r;
                    perror("rle_decode_file: fwrite");
                    goto exit;
                }

                decoded_total += decoded;
            }
        }
        assert(readlen == 0);
        assert(feof(src));
        r = 0;
    }

exit:
    DBG("return %d\n", r);
    return r;
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
