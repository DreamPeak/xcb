lib_LTLIBRARIES = md_ctp.la
md_ctp_la_SOURCES = ctp.cpp md_ctp.c
md_ctp_la_CPPFLAGS = -I$(top_srcdir)/include
md_ctp_la_CFLAGS = -fPIC -Wall
md_ctp_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/ctp -lthostmduserapi -ltcmalloc

ctpdir = $(libdir)
ctp_DATA = libthostmduserapi.so libthosttraderapi.so

