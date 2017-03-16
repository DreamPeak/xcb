sbin_PROGRAMS = xcb-ctp
xcb_ctp_SOURCES = $(top_srcdir)/src/mem.c $(top_srcdir)/src/dstr.c             \
$(top_srcdir)/src/utils.c $(top_srcdir)/src/logger.c $(top_srcdir)/src/config.c\
xcb-ctp.cpp
xcb_ctp_CPPFLAGS = -I$(top_srcdir)/include
xcb_ctp_CFLAGS = -Wall -Winline -pthread
xcb_ctp_LDADD = libthosttraderapi.so
xcb_ctp_LDFLAGS = -pthread -ltcmalloc

lib_LTLIBRARIES = md_ctp.la
md_ctp_la_SOURCES = ctp.cpp md_ctp.c
md_ctp_la_CPPFLAGS = -I$(top_srcdir)/include
md_ctp_la_CFLAGS = -fPIC -Wall
md_ctp_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/ctp -lthostmduserapi -ltcmalloc

ctpdir = $(libdir)
ctp_DATA = libthostmduserapi.so libthosttraderapi.so

