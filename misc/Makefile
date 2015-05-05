#
# Copyright (c) 2013-2015, Dalian Futures Information Technology Co., Ltd.
#
# Guodong Zhang <zhangguodong at dce dot com dot cn>
# Xiaoye Meng   <mengxiaoye at dce dot com dot cn>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

C++      = g++
CC       = gcc
CPPFLAGS = -I../include/
CFLAGS   = -fPIC -Wall -march=corei7 -g -O2
LDFLAGS  = -ltcmalloc
TARGET   = app_ema.so
OBJ1     = stl.o
OBJ2     = app_ema.o

all : $(TARGET)

$(TARGET) : $(OBJ1) $(OBJ2)
	$(C++) -shared $^ $(LDFLAGS) -Wl,-soname -Wl,$@ -o $@

$(OBJ1) : stl.cpp
	$(C++) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ2) : app_ema.c
	$(CC)  $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all clean

clean:
	rm -f $(TARGET) $(OBJ1) $(OBJ2)

