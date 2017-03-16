sbin_PROGRAMS = xcb-xspeed
xcb_xspeed_SOURCES = $(top_srcdir)/src/mem.c $(top_srcdir)/src/dstr.c          \
$(top_srcdir)/src/utils.c $(top_srcdir)/src/logger.c $(top_srcdir)/src/config.c\
xcb-xspeed.cpp
xcb_xspeed_CPPFLAGS = -I$(top_srcdir)/include
xcb_xspeed_CFLAGS = -Wall -Winline -pthread
xcb_xspeed_LDADD = libDFITCTraderApi.so
xcb_xspeed_LDFLAGS = -pthread -ltcmalloc

lib_LTLIBRARIES = md_xspeed.la
md_xspeed_la_SOURCES = xspeed.cpp md_xspeed.c
md_xspeed_la_CPPFLAGS = -I$(top_srcdir)/include
md_xspeed_la_CFLAGS = -fPIC -Wall
md_xspeed_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/xspeed -lDFITCMdApi -ltcmalloc

xspeeddir = $(libdir)
xspeed_DATA = libDFITCTraderApi.so libDFITCMdApi.so

