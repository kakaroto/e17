# GPL (C) 1997 The Rasterman
#   raster@rasterman.com

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# the resultant executable
lib_LTLIBRARIES = libFnlib.la

include_HEADERS = Fnlib.h Fnlib_types.h

libFnlib_la_SOURCES = \
	file.c		\
	file.h		\
	priv.c		\
	misc.c

libFnlib_la_LDFLAGS = -version-info 5:0:5
libFnlib_la_LIBADD  = @IMLIB_LIBS@

DEFS = -DSYSTEM_FNRC=\""@sysconfdir@/fnrc"\"

INCLUDES = \
	-I$(srcdir)	\
	-I$(top_srcdir) \
	@IMLIB_CFLAGS@	\
	-I$(includedir)
