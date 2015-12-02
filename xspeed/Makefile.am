sbin_PROGRAMS = xcb-exp
xcb_exp_SOURCES = $(top_srcdir)/src/mem.c $(top_srcdir)/src/dstr.c             \
$(top_srcdir)/src/utilities.c $(top_srcdir)/src/logger.c                       \
$(top_srcdir)/src/config.c xcb-exp.cpp
xcb_exp_CPPFLAGS = -I$(top_srcdir)/include
xcb_exp_CFLAGS = -Wall -Winline
xcb_exp_LDADD = libDFITCTraderApi.so
xcb_exp_LDFLAGS = -ltcmalloc

lib_LTLIBRARIES = md_xspeed.la
md_xspeed_la_SOURCES = xspeed.cpp md_xspeed.c
md_xspeed_la_CPPFLAGS = -I$(top_srcdir)/include
md_xspeed_la_CFLAGS = -fPIC -Wall
md_xspeed_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/xspeed -lDFITCMdApi -ltcmalloc

xspeeddir = $(libdir)
xspeed_DATA = libDFITCMdApi.so

