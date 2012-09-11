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
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = -D_FORTIFY_SOURCE=2 -D_BSD_SOURCE -D_ISO_C11_SOURCE -D_FILE_OFFSET_BITS=64
AM_CFLAGS = -Wall -Werror -Wextra -std=c11
AM_LDFLAGS = -std=c11

lib_LTLIBRARIES = librle-@RLE_API_VERSION@.la
librle_@RLE_API_VERSION@_la_SOURCES = rle/rle.c rle/rle-debug.c
librle_@RLE_API_VERSION@_la_CFLAGS = $(AM_CFLAGS) -fvisibility=hidden
librle_@RLE_API_VERSION@_la_LDFLAGS = $(AM_LDFLAGS) -fvisibility=hidden -version-info $(RLE_SO_VERSION)
rle_includedir = $(includedir)/rle-$(RLE_API_VERSION)
nobase_rle_include_HEADERS = rle.h rle/rle.h
rle_libincludedir = $(libdir)/rle-$(RLE_API_VERSION)/include
nodist_rle_libinclude_HEADERS = rleconfig.h

check_PROGRAMS = tests/easy tests/simple

tests_easy_SOURCES = tests/easy.c
tests_easy_LDADD = librle-@RLE_API_VERSION@.la
tests_easy_LDFLAGS = $(AM_LDFLAGS) -no-install

tests_simple_SOURCES = tests/simple.c
tests_simple_LDADD = librle-@RLE_API_VERSION@.la
tests_simple_LDFLAGS = $(AM_LDFLAGS) -no-install

TESTS = $(check_PROGRAMS)
