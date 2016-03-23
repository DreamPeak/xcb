sbin_PROGRAMS = xcb-exp2
xcb_exp2_SOURCES = $(top_srcdir)/src/mem.c $(top_srcdir)/src/dstr.c            \
$(top_srcdir)/src/utils.c $(top_srcdir)/src/logger.c $(top_srcdir)/src/config.c\
xcb-exp2.cpp
xcb_exp2_CPPFLAGS = -I$(top_srcdir)/include
xcb_exp2_CFLAGS = -Wall -Winline
xcb_exp2_LDADD = libDFITCSECTraderApi.so
xcb_exp2_LDFLAGS = -ltcmalloc

lib_LTLIBRARIES = md_sec.la
md_sec_la_SOURCES = sec.cpp md_sec.c
md_sec_la_CPPFLAGS = -I$(top_srcdir)/include
md_sec_la_CFLAGS = -fPIC -Wall
md_sec_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/sec -lDFITCSECMdApi -ltcmalloc

secdir = $(libdir)
sec_DATA = libDFITCSECTraderApi.so libDFITCSECMdApi.so

