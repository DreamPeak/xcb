C++      = g++
CC       = gcc
CPPFLAGS = -I../include/
CFLAGS   = -fPIC -Wall -march=corei7 -g -O2
LDFLAGS1 = thostmduserapi.so -ltcmalloc
LDFLAGS2 = ctp.so
TARGET1  = ctp.so
TARGET2  = md_ctp.so
OBJ1     = ctp.o
OBJ2     = md_ctp.o

all : $(TARGET1) $(TARGET2)

$(TARGET1) : $(OBJ1)
	$(C++) -shared $^ $(LDFLAGS1) -Wl,-soname -Wl,$@ -o $@

$(TARGET2) : $(OBJ2)
	$(CC)  -shared $^ $(LDFLAGS2) -Wl,-soname -Wl,$@ -o $@

$(OBJ1) : ctp.cpp
	$(C++) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ2) : md_ctp.c
	$(CC)  $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: all clean

clean:
	rm -f $(TARGET1) $(TARGET2) $(OBJ1) $(OBJ2)

