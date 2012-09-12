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
#include "orle.h"
#include "orle-debug.h"
#include "orle-io.h"

int orle_encode_file(FILE * const dst, FILE * const src) {
    DBG("enter\n");
    assert(dst);
    assert(src);

    int r;
    const size_t pagesize = sysconf(_SC_PAGESIZE);
    const size_t encbuflen = orle_encode_size_bound(pagesize);
    {
        unsigned char decbuf[pagesize];
        unsigned char encbuf[encbuflen];
        size_t readlen;
        while ((readlen = fread(decbuf, 1, pagesize, src)) > 0) {
            if (readlen < pagesize) {
                if ((r = ferror(src))) {
                    // LCOV_EXCL_START
                    errno = r;
                    perror("orle_encode_file: fread");
                    goto exit;
                }   // LCOV_EXCL_STOP
            }
            size_t enclen = orle_encode_size_bound(readlen);
            size_t encoded = orle_encode_bytes(encbuf, &enclen, decbuf, readlen);
            if (encoded < readlen) {
                // LCOV_EXCL_START
                perror("orle_encode_file: orle_encode_bytes");
                r = -1;
                goto exit;
            }   // LCOV_EXCL_STOP
            size_t writelen = fwrite(encbuf, 1, enclen, dst);
            if (writelen < enclen) {
                // LCOV_EXCL_START
                r = ferror(dst);
                assert(r);
                errno = r;
                perror("orle_encode_file: fwrite");
                goto exit;
            }   // LCOV_EXCL_STOP

        }
        assert(readlen == 0);
        assert(feof(src));
        r = 0;
    }

exit:
    DBG("return %d\n", r);
    return r;
}

int orle_decode_file(FILE * const dst, FILE * const src) {
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
                    // LCOV_EXCL_START
                    errno = r;
                    perror("orle_decode_file: fread");
                    goto exit;
                }   // LCOV_EXCL_STOP
            }
            size_t enclen = readlen + leftover;
            leftover = 0;
            DBG("decoding chunk of %zu...\n", enclen);
            size_t decoded_total = 0;
            while (decoded_total < enclen) {
                DBG("decoded %zu/%zu\n", decoded_total, enclen);
                size_t decoded;
                size_t declen = pagesize;
                decoded = orle_decode_bytes(decbuf, &declen,
                                           &encbuf[decoded_total], enclen - decoded_total);
                assert(declen <= pagesize);
                if (decoded < enclen - decoded_total) {
                    if (errno != ENOBUFS) {
                        // LCOV_EXCL_START
                        perror("orle_decode_file: orle_decode_bytes");
                        r = -1;
                        goto exit;
                    }   // LCOV_EXCL_STOP
                }
                if (decoded == 0) {
                    memmove(&encbuf[0], &encbuf[decoded_total], enclen - decoded_total);
                    leftover = enclen - decoded_total;
                    break;
                }

                size_t writelen = fwrite(decbuf, 1, declen, dst);
                if (writelen < declen) {
                    // LCOV_EXCL_START
                    r = ferror(dst);
                    assert(r);
                    errno = r;
                    perror("orle_decode_file: fwrite");
                    goto exit;
                }   // LCOV_EXCL_STOP

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

// LCOV_EXCL_START
__attribute__((nonnull(1)))
static inline void dbg_perror(char const * const str) {
    if (orle_debug) {
        perror(str);
    }
}
// LCOV_EXCL_STOP

__attribute__((warn_unused_result, nonnull(3,4)))
static int fdopen_dst_src(int dst, int src, FILE ** const dstfp, FILE ** const srcfp) {
    int r;
    int newdst = dup(dst);
    if (newdst < 0) {
        r = errno;
        goto exit;
    }
    FILE *dstf = fdopen(newdst, "a");
    if (!dstf) {
        r = errno;
        int cr = close(newdst);
        if (cr) {
            dbg_perror("fdopen_dst_src: close"); // LCOV_EXCL_LINE
        }
        goto exit;
    }
    int newsrc = dup(src);
    if (newsrc < 0) {
        r = errno;
        int cr = fclose(dstf);
        if (cr) {
            dbg_perror("fdopen_dst_src: fclose"); // LCOV_EXCL_LINE
        }
        goto exit;
    }
    FILE *srcf = fdopen(newsrc, "r");
    if (!srcf) {
        r = errno;
        int cr = close(newsrc);
        if (cr) {
            dbg_perror("fdopen_dst_src: close"); // LCOV_EXCL_LINE
        }
        cr = fclose(dstf);
        if (cr) {
            dbg_perror("fdopen_dst_src: fclose"); // LCOV_EXCL_LINE
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
        errsv1 = errno; // LCOV_EXCL_LINE
    } else {
        *dstf = NULL;
    }
    r2 = fclose(*srcf);
    if (r2) {
        errsv2 = errno; // LCOV_EXCL_LINE
    } else {
        *srcf = NULL;
    }
    if (r1 && r2) {
        // LCOV_EXCL_START
        errno = errsv2;
        dbg_perror("fclose_dst_src: failed to close srcf, can't express this error"); // LCOV_EXCL_LINE
        errno = errsv1;
        r = r1;
        // LCOV_EXCL_STOP
    } else if (r1) {
        r = r1; // LCOV_EXCL_LINE
        errno = errsv1; // LCOV_EXCL_LINE
    } else if (r2) {
        r = r2; // LCOV_EXCL_LINE
        errno = errsv2; // LCOV_EXCL_LINE
    } else {
        r = 0;
    }
    return r;
}

int orle_encode_fd(int dst, int src) {
    int r, cr;
    FILE *dstf, *srcf;
    r = fdopen_dst_src(dst, src, &dstf, &srcf);
    if (r) {
        goto exit;
    }

    r = orle_encode_file(dstf, srcf);

    cr = fclose_dst_src(&dstf, &srcf);
    if (!r && cr) {
        r = cr; // LCOV_EXCL_LINE
    }
exit:
    return r;
}

int orle_decode_fd(int dst, int src) {
    int r, cr;
    FILE *dstf, *srcf;
    r = fdopen_dst_src(dst, src, &dstf, &srcf);
    if (r) {
        goto exit;
    }

    r = orle_decode_file(dstf, srcf);

    cr = fclose_dst_src(&dstf, &srcf);
    if (!r && cr) {
        r = cr; // LCOV_EXCL_LINE
    }
exit:
    return r;
}
