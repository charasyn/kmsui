#
#  Copyright (c) 2012 Rob Clark <rob@ti.com>
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice (including the next
#  paragraph) shall be included in all copies or substantial portions of the
#  Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#  SOFTWARE.
#

# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([kmscube], [0.0.1], [https://cgit.freedesktop.org/mesa/kmscube/], [kmscube])
AC_CONFIG_AUX_DIR([build-aux])

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2])

AC_PROG_CC

# Enable quiet compiles on automake 1.11.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Obtain compiler/linker options for depedencies
PKG_CHECK_MODULES(DRM, [libdrm >= 2.4.71])
PKG_CHECK_MODULES(GBM, gbm >= 13.0)
PKG_CHECK_MODULES(EGL, egl)
PKG_CHECK_MODULES(GLES, glesv1_cm)

AC_CHECK_LIB([gbm], [gbm_bo_get_modifier], [gbm_modifiers=yes], [])

AC_ARG_ENABLE([gbm-modifiers],
	      [AS_HELP_STRING([--enable-gbm-modifiers],
	          [enable using GBM modifiers @<:@default=auto@:>@])],
	      [enable_gbm_modifiers="$enableval"],
	      [enable_gbm_modifiers=auto])

if test "x$enable_gbm_modifiers" = xyes -a "x$gbm_modifiers" != xyes; then
	AC_MSG_ERROR([Cannot find gbm modifier supported mesa])
fi

if test "x$gbm_modifiers" = xyes; then
	AC_DEFINE(HAVE_GBM_MODIFIERS, 1, [Define if you can use GBM properties.])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
