CC=gcc
LINK=ar rcs

_INCDIRS=include
INCDIRS=$(addprefix -I,$(_INCDIRS))

_LIBDIRS=lib
LIBDIRS=$(addprefix -L,$(_LIBDIRS))

_LIBS=
LIBS=$(addprefix -l,$(_LIBS))

CFLAGS=-O3 -Wall $(INCDIRS) -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC
LDFLAGS=

SRCDIR=src
_SRCFILES=sensirion_shdlc.c  sensirion_uart.c  sps30.c  sps_git_version.c
SRCS=$(addprefix $(SRCDIR)/,$(_SRCFILES))

INCDIR=include/sps
_INCFILES=$(_SRCFILES:%.c=%.h) sensirion_arch_config.h
INCS=$(addprefix $(INCDIR)/,$(_INCFILES))

OBJDIR=build
_OBJFILES=$(_SRCFILES:%.c=%.o)
OBJS=$(addprefix $(OBJDIR)/,$(_OBJFILES))

LIBDIR=lib
LIBNAME=libsps.a

DIRGUARD=@mkdir -p $(@D)

all: $(LIBDIR)/$(LIBNAME)

$(LIBDIR)/$(LIBNAME) : $(OBJS)
	$(DIRGUARD)
	$(LINK) $@ $(OBJS) $(LDFLAGS)
	cd ./pcount && $(MAKE)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(DIRGUARD)
	$(CC) $< -c -o $@ $(CFLAGS)

.SECONDARY: $(OBJS)
.PHONY: clean

install:
	cp bin/pcount /usr/bin

uninstall:
	rm /usr/bin/pcount

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(LIBDIR)/*.a
	rmdir $(OBJDIR) $(LIBDIR)
	cd ./pcount && $(MAKE) clean
