sbin_PROGRAMS = xcb-sec
xcb_sec_SOURCES = $(top_srcdir)/src/mem.c $(top_srcdir)/src/dstr.c             \
$(top_srcdir)/src/utils.c $(top_srcdir)/src/logger.c $(top_srcdir)/src/config.c\
xcb-sec.cpp
xcb_sec_CPPFLAGS = -I$(top_srcdir)/include
xcb_sec_CFLAGS = -Wall -Winline -pthread
xcb_sec_LDADD = libDFITCSECTraderApi.so
xcb_sec_LDFLAGS = -pthread -ltcmalloc

lib_LTLIBRARIES = md_sec.la
md_sec_la_SOURCES = sec.cpp md_sec.c
md_sec_la_CPPFLAGS = -I$(top_srcdir)/include
md_sec_la_CFLAGS = -fPIC -Wall
md_sec_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/sec -lDFITCSECMdApi -ltcmalloc

secdir = $(libdir)
sec_DATA = libDFITCSECTraderApi.so libDFITCSECMdApi.so

