CC=gcc

_INCDIRS=../include
INCDIRS=$(addprefix -I,$(_INCDIRS))

_LIBDIRS=../lib
LIBDIRS=$(addprefix -L,$(_LIBDIRS))
_COMMA=,
RPATH_LIBDIRS=$(addprefix -Wl$(_COMMA)-rpath$(_COMMA),$(_LIBDIRS))

_LIBS=libsps.a
LIBS=$(addprefix -l:,$(_LIBS))

CFLAGS=-O3 -Wall $(INCDIRS) -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
LDFLAGS=-O3 $(INCDIRS) $(LIBDIRS) $(LIBS) $(RPATH_LIBDIRS)
_SRCFILES=pcount.c
.SRCS=$(_SRCFILES)

_OBJFILES=$(_SRCFILES:%.c=%.o)
OBJS=$(_OBJFILES)

EXE=pcount

LIBNAME=libsps.a

DIRGUARD=@mkdir -p $(@D)
BINDIR=../bin

DATADIR=data

all: $(BINDIR)/$(EXE) directories

$(BINDIR)/%: $(OBJS)
	$(DIRGUARD)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) $< -c -o $@ $(CFLAGS)

directories : $(DATADIR)

$(DATADIR):
	mkdir -p $(DATADIR)

.SECONDARY: $(OBJS) $(DATADIR)
.PHONY: clean directories

clean:
	rm -f *.o
	rm -f $(BINDIR)/$(EXE)
	rmdir $(BINDIR)
