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

#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <orle.h>

#include "../unittest.h"

void setup(void) {
    int r;
    FILE *srcf = fopen("/tmp/orle-io-syscall.src", "w");
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
}

void basic(void) {
    int r;

    int srcfd = open("/tmp/orle-io-syscall.src", O_RDONLY);
    expect(srcfd >= 0);
    int encfd = open("/tmp/orle-io-syscall.enc", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    expect(encfd >= 0);
    r = orle_encode_fd(encfd, srcfd);
    assert(!r);
    struct stat srcst, encst;
    r = fstat(srcfd, &srcst);
    expect(!r);
    r = close(srcfd);
    expect(!r);
    r = fstat(encfd, &encst);
    expect(!r);
    r = close(encfd);
    expect(!r);

    assert(encst.st_size < srcst.st_size);

    encfd = open("/tmp/orle-io-syscall.enc", O_RDONLY);
    expect(encfd >= 0);
    int decfd = open("/tmp/orle-io-syscall.dec", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    expect(decfd >= 0);
    r = orle_decode_fd(decfd, encfd);
    assert(!r);
    r = close(encfd);
    expect(!r);
    r = close(decfd);
    expect(!r);

    r = system("diff -q /tmp/orle-io-syscall.src /tmp/orle-io-syscall.dec");
    assert(!r);
}

enum badness {
    FD_IS_OK,
    FD_IS_BULLSHIT,
    FD_IS_WRONGMODE
};

void badenc(enum badness encbad, enum badness srcbad) {
    int r;
    // gcc doesn't understand that the switch statements are ok
    int src = -1, enc = -1;

    switch (srcbad) {
    case FD_IS_OK:
        src = open("/tmp/orle-io-syscall.src", O_RDONLY);
        expect(src >= 0);
        break;
    case FD_IS_BULLSHIT:
        src = 1000;
        break;
    case FD_IS_WRONGMODE:
        src = open("/tmp/orle-io-syscall.src", O_WRONLY);
        expect(src >= 0);
        break;
    }
    switch (encbad) {
    case FD_IS_OK:
        enc = open("/tmp/orle-io-syscall.enc", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        expect(enc >= 0);
        break;
    case FD_IS_BULLSHIT:
        enc = 1001;
        break;
    case FD_IS_WRONGMODE:
        enc = open("/tmp/orle-io-syscall.enc", O_RDONLY);
        expect(enc >= 0);
        break;
    }

    r = orle_encode_fd(enc, src);
    if (srcbad == FD_IS_OK && encbad == FD_IS_OK) {
        assert(!r);
    } else {
        assert(r);
    }
}

void baddec(enum badness decbad, enum badness encbad) {
    int r;
    int enc = -1, dec = -1;

    switch (encbad) {
    case FD_IS_OK:
        enc = open("/tmp/orle-io-syscall.enc", O_RDONLY);
        expect(enc >= 0);
        break;
    case FD_IS_BULLSHIT:
        enc = 1000;
        break;
    case FD_IS_WRONGMODE:
        enc = open("/tmp/orle-io-syscall.enc", O_WRONLY);
        expect(enc >= 0);
        break;
    }
    switch (decbad) {
    case FD_IS_OK:
        dec = open("/tmp/orle-io-syscall.dec", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        expect(dec >= 0);
        break;
    case FD_IS_BULLSHIT:
        dec = 1001;
        break;
    case FD_IS_WRONGMODE:
        dec = open("/tmp/orle-io-syscall.dec", O_RDONLY);
        expect(dec >= 0);
        break;
    }

    r = orle_decode_fd(dec, enc);
    if (encbad == FD_IS_OK && decbad == FD_IS_OK) {
        assert(!r);
    } else {
        assert(r);
    }
}

int main(void) {
    setup();
    basic();

    badenc(FD_IS_OK, FD_IS_OK);
    badenc(FD_IS_OK, FD_IS_BULLSHIT);
    badenc(FD_IS_OK, FD_IS_WRONGMODE);
    badenc(FD_IS_BULLSHIT, FD_IS_OK);
    badenc(FD_IS_BULLSHIT, FD_IS_BULLSHIT);
    badenc(FD_IS_BULLSHIT, FD_IS_WRONGMODE);
    badenc(FD_IS_WRONGMODE, FD_IS_OK);
    badenc(FD_IS_WRONGMODE, FD_IS_BULLSHIT);
    badenc(FD_IS_WRONGMODE, FD_IS_WRONGMODE);
    baddec(FD_IS_OK, FD_IS_OK);
    baddec(FD_IS_OK, FD_IS_BULLSHIT);
    baddec(FD_IS_OK, FD_IS_WRONGMODE);
    baddec(FD_IS_BULLSHIT, FD_IS_OK);
    baddec(FD_IS_BULLSHIT, FD_IS_BULLSHIT);
    baddec(FD_IS_BULLSHIT, FD_IS_WRONGMODE);
    baddec(FD_IS_WRONGMODE, FD_IS_OK);
    baddec(FD_IS_WRONGMODE, FD_IS_BULLSHIT);
    baddec(FD_IS_WRONGMODE, FD_IS_WRONGMODE);

    unittest_finish();
}
