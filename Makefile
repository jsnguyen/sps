C=gcc

_INCDIRS=include
INCDIRS=$(addprefix -I,$(_INCDIRS))

_LIBDIRS=lib
LIBDIRS=$(addprefix -L,$(_LIBDIRS))

_LIBS=
LIBS=$(addprefix -l,$(_LIBS))

CFLAGS=-O3 -Wall $(INCDIRS) -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC -std=gnu99
LDFLAGS=-O3 -shared $(INCDIRS) $(LIBDIRS) $(LIBS)

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
LIBNAME=libsps.so

DIRGUARD=@mkdir -p $(@D)

all: $(LIBDIR)/$(LIBNAME)

$(LIBDIR)/$(LIBNAME): $(OBJS)
	$(DIRGUARD)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	cd ./pcount && $(MAKE)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(DIRGUARD)
	$(CC) $< -c -o $@ $(CFLAGS)

.SECONDARY: $(OBJS)
.PHONY: clean

PREFIX=/usr/local
SUFFIX=/sps
install: $(LIBDIR)/$(LIBNAME)
	install -d $(DESTDIR)$(PREFIX)/lib$(SUFFIX)
	install -m 644 $(LIBDIR)/$(LIBNAME) $(DESTDIR)$(PREFIX)/lib$(SUFFIX)
	install -d $(DESTDIR)$(PREFIX)/include$(SUFFIX)
	install -m 644 $(INCS) $(DESTDIR)$(PREFIX)/include$(SUFFIX)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/include$(SUFFIX)/*.h
	rm -f $(DESTDIR)$(PREFIX)/lib$(SUFFIX)/*.so
	rmdir $(DESTDIR)$(PREFIX)/include$(SUFFIX)/
	rmdir $(DESTDIR)$(PREFIX)/lib$(SUFFIX)/

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(LIBDIR)/*.so
	rmdir $(OBJDIR) $(LIBDIR)
	cd ./pcount && $(MAKE) clean
