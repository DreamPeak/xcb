lib_LTLIBRARIES = md_femas.la
md_femas_la_SOURCES = femas.cpp md_femas.c
md_femas_la_CPPFLAGS = -I$(top_srcdir)/include
md_femas_la_CFLAGS = -fPIC -Wall
md_femas_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/femas -lUSTPmduserapi -ltcmalloc

femasdir = $(libdir)
femas_DATA = libUSTPmduserapi.so
