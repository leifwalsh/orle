dnl Copyright (c) 2012, Leif Walsh
dnl All rights reserved.
dnl
dnl Redistribution and use in source and binary forms, with or without
dnl modification, are permitted provided that the following conditions are met:
dnl     * Redistributions of source code must retain the above copyright
dnl       notice, this list of conditions and the following disclaimer.
dnl     * Redistributions in binary form must reproduce the above copyright
dnl       notice, this list of conditions and the following disclaimer in the
dnl       documentation and/or other materials provided with the distribution.
dnl     * Neither the name of the <organization> nor the
dnl       names of its contributors may be used to endorse or promote products
dnl       derived from this software without specific prior written permission.
dnl
dnl THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
dnl ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
dnl WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
dnl DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
dnl DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
dnl (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
dnl LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
dnl ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
dnl (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
dnl SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

AC_INIT([rle], [0.0.4], [leif.walsh@gmail.com], [rle], [http://github.com/adlaiff6/rle])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([foreign color-tests parallel-tests])
AM_SILENT_RULES([yes])
AC_CONFIG_HEADERS([config.h rleconfig.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AM_PROG_CC_C_O
LT_INIT()
AC_SUBST([RLE_SO_VERSION], [1:0:0])
AC_SUBST([RLE_API_VERSION], [1.0])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
