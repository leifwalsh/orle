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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rle.h>

#include "../unittest.h"

int main(void) {
    int r;
    FILE *srcf = fopen("/tmp/rle-io-simple.src", "w");
    expect(srcf);
    const size_t pagesize = sysconf(_SC_PAGESIZE);
    char buf[pagesize];
    for (size_t fsize = 0; fsize < (1 << 22); fsize += pagesize) {
        if ((fsize / pagesize) % 2 == 0) {
            memset(buf, 0, pagesize);
        } else {
            uint32_t *intbuf = (uint32_t *) buf;
            for (size_t i = 0; i < pagesize / sizeof(uint32_t); ++i) {
                intbuf[i] = random();
            }
        }
        size_t wrote = fwrite(buf, 1, pagesize, srcf);
        expect(wrote == pagesize);
    }
    r = fclose(srcf);
    expect(!r);

    srcf = fopen("/tmp/rle-io-simple.src", "r");
    expect(srcf);
    FILE *encf = fopen("/tmp/rle-io-simple.enc", "w");
    expect(encf);
    r = rle_encode_file(encf, srcf);
    assert(!r);
    r = fclose(srcf);
    expect(!r);
    r = fclose(encf);
    expect(!r);

    encf = fopen("/tmp/rle-io-simple.enc", "r");
    expect(encf);
    FILE *decf = fopen("/tmp/rle-io-simple.dec", "w");
    expect(decf);
    r = rle_decode_file(decf, encf);
    assert(!r);
    r = fclose(encf);
    expect(!r);
    r = fclose(decf);
    expect(!r);

    r = system("diff -q /tmp/rle-io-simple.src /tmp/rle-io-simple.dec");
    assert(!r);

    unittest_finish();
}
