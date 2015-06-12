lib_LTLIBRARIES = md_xspeed2.la
md_xspeed2_la_SOURCES = xspeed2.cpp md_xspeed2.c
md_xspeed2_la_CPPFLAGS = -I$(top_srcdir)/include
md_xspeed2_la_CFLAGS = -fPIC -Wall
md_xspeed2_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/xspeed2 -llevel2Api -ltcmalloc

xspeed2dir = $(libdir)
xspeed2_DATA = liblevel2Api.so

