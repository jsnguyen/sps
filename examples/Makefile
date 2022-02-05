CC=gcc

_INCDIRS=../include
INCDIRS=$(addprefix -I,$(_INCDIRS))

_LIBDIRS=../lib
LIBDIRS=$(addprefix -L,$(_LIBDIRS))

_LIBS=sps
LIBS=$(addprefix -l,$(_LIBS))

CFLAGS=-O3 -Wall $(INCDIRS) -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
LDFLAGS=-O3 $(INCDIRS) $(LIBDIRS) $(LIBS) -Wl,-rpath,$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../lib

_SRCFILES=sps30_example_usage.c
.SRCS=$(_SRCFILES)

_OBJFILES=$(_SRCFILES:%.c=%.o)
OBJS=$(_OBJFILES)

EXES=sps30_example_usage.exe

LIBNAME=libsps.so

DIRGUARD=@mkdir -p $(@D)

all: $(EXES)

%.exe: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) $< -c -o $@ $(CFLAGS)

.SECONDARY: $(OBJS)
.PHONY: clean

clean:
	rm *.o
	rm *.exe

