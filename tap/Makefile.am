lib_LTLIBRARIES = md_tap.la
md_tap_la_SOURCES = tap.cpp md_tap.c
md_tap_la_CPPFLAGS = -I$(top_srcdir)/include
md_tap_la_CFLAGS = -fPIC -Wall
md_tap_la_LDFLAGS = -module -avoid-version -L$(top_srcdir)/tap -lTapQuoteAPI -ltcmalloc

tapdir = $(libdir)
tap_DATA = libTapQuoteAPI.so

