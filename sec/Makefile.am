lib_LTLIBRARIES = md_sec.la
md_sec_la_SOURCES = sec.cpp md_sec.c
md_sec_la_CPPFLAGS = -I$(top_srcdir)/include
md_sec_la_CFLAGS = -fPIC -Wall
md_sec_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/sec -lDFITCSECMdApi -ltcmalloc

secdir = $(libdir)
sec_DATA = libDFITCSECMdApi.so

