# Copyright (c) 2012, Leif Walsh
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the <organization> nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

AM_CPPFLAGS = -D_FORTIFY_SOURCE=2 -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -D_ISO_C11_SOURCE -D_FILE_OFFSET_BITS=64
AM_CFLAGS = -Wall -Werror -Wextra -std=c11
AM_LDFLAGS = -std=c11

lib_LTLIBRARIES = liborle-@ORLE_API_VERSION@.la
liborle_@ORLE_API_VERSION@_la_SOURCES = orle/orle.c orle/orle-io.c orle/orle-debug.c
liborle_@ORLE_API_VERSION@_la_CFLAGS = $(AM_CFLAGS) -fvisibility=hidden
liborle_@ORLE_API_VERSION@_la_LDFLAGS = $(AM_LDFLAGS) -fvisibility=hidden -version-info $(ORLE_SO_VERSION)
orle_includedir = $(includedir)/orle-$(ORLE_API_VERSION)
nobase_orle_include_HEADERS = orle.h orle/orle.h orle/orle-io.h
orle_libincludedir = $(libdir)/orle-$(ORLE_API_VERSION)/include
nodist_orle_libinclude_HEADERS = orleconfig.h

check_PROGRAMS = \
    tests/encoder/easy tests/encoder/osc tests/encoder/simple \
    tests/io/simple tests/io/syscall

tests_encoder_easy_SOURCES = tests/encoder/easy.c
tests_encoder_easy_LDADD = liborle-@ORLE_API_VERSION@.la
tests_encoder_easy_LDFLAGS = $(AM_LDFLAGS) -no-install

tests_encoder_osc_SOURCES = tests/encoder/osc.c
tests_encoder_osc_LDADD = liborle-@ORLE_API_VERSION@.la
tests_encoder_osc_LDFLAGS = $(AM_LDFLAGS) -no-install

tests_encoder_simple_SOURCES = tests/encoder/simple.c
tests_encoder_simple_LDADD = liborle-@ORLE_API_VERSION@.la
tests_encoder_simple_LDFLAGS = $(AM_LDFLAGS) -no-install

tests_io_simple_SOURCES = tests/io/simple.c
tests_io_simple_LDADD = liborle-@ORLE_API_VERSION@.la
tests_io_simple_LDFLAGS = $(AM_LDFLAGS) -no-install

tests_io_syscall_SOURCES = tests/io/syscall.c
tests_io_syscall_LDADD = liborle-@ORLE_API_VERSION@.la
tests_io_syscall_LDFLAGS = $(AM_LDFLAGS) -no-install

TESTS = $(check_PROGRAMS)

%.gcda: %.lo
.PHONY: all.info
all.info: $(patsubst %.c,%.gcda,$(OBJECTS))
	lcov -b $(abs_srcdir) -d $(abs_srcdir) -z
	@$(MAKE) check
	lcov -b $(abs_srcdir) -d $(abs_srcdir) -c -o $@
orle.info: all.info
	lcov -e $< "$(abs_srcdir)/orle/*" -o $@
cov/index.html: orle.info
	genhtml -p $(abs_srcdir) -o $(dir $@) $<
coverage: cov/index.html
.PHONY: coverage
