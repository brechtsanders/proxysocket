ifeq ($(OS),)
OS = $(shell uname -s)
ifneq ($(findstring Windows,$(OS))$(findstring MINGW32,$(OS))$(findstring MSYS,$(OS)),)
OS = Windows_NT
endif
endif
PREFIX = /usr/local
CC   = gcc
CPP  = g++
AR   = ar
LIBPREFIX = lib
LIBEXT = .a
ifeq ($(OS),Windows_NT)
BINEXT = .exe
SOEXT = .dll
else ifeq ($(OS),Darwin)
BINEXT =
SOEXT = .dylib
else
BINEXT =
SOEXT = .so
endif
INCS = -Iinclude -Isrc
CFLAGS = $(INCS) -Os
CPPFLAGS = $(INCS) -Os
STATIC_CFLAGS = -DBUILD_PROXYSOCKET_STATIC
SHARED_CFLAGS = -DBUILD_PROXYSOCKET_DLL
LIBS =
LDFLAGS =
ifeq ($(OS),Darwin)
CFLAGS += -I/opt/local/include -I/opt/local/lib/libzip/include
LDFLAGS += -L/opt/local/lib
#CFLAGS += -arch i386 -arch x86_64
#LDFLAGS += -arch i386 -arch x86_64
STRIPFLAG =
else
STRIPFLAG = -s
endif
MKDIR = mkdir -p
RM = rm -f
RMDIR = rm -rf
CP = cp -f
CPDIR = cp -rf
DOXYGEN := $(shell which doxygen)

PROXYSOCKET_OBJ = src/proxysocket.o
PROXYSOCKET_LDFLAGS =
PROXYSOCKET_SHARED_LDFLAGS =
ifneq ($(OS),Windows_NT)
SHARED_CFLAGS += -fPIC
endif
ifeq ($(OS),Windows_NT)
PROXYSOCKET_SHARED_LDFLAGS += -Wl,--out-implib,$@$(LIBEXT) -lws2_32
PROXYSOCKET_LDFLAGS += -lws2_32
endif
ifeq ($(OS),Darwin)
OS_LINK_FLAGS = -dynamiclib -o $@
else
OS_LINK_FLAGS = -shared -Wl,-soname,$@ $(STRIPFLAG)
endif

TOOLS_BIN = ipify$(BINEXT)
EXAMPLES_BIN = proxysocket_test$(BINEXT)

COMMON_PACKAGE_FILES = README.md LICENSE.txt Changelog.txt
SOURCE_PACKAGE_FILES = $(COMMON_PACKAGE_FILES) Makefile doc/Doxyfile src/*.h src/*.c examples/*.c build/*.cbp

default: all

all: static-lib shared-lib tools

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

%.static.o: %.c
	$(CC) -c -o $@ $< $(STATIC_CFLAGS) $(CFLAGS) 

%.shared.o: %.c
	$(CC) -c -o $@ $< $(SHARED_CFLAGS) $(CFLAGS)

static-lib: $(LIBPREFIX)proxysocket$(LIBEXT)

shared-lib: $(LIBPREFIX)proxysocket$(SOEXT)

$(LIBPREFIX)proxysocket$(LIBEXT): $(PROXYSOCKET_OBJ:%.o=%.static.o)
	$(AR) cru $@ $^

$(LIBPREFIX)proxysocket$(SOEXT): $(PROXYSOCKET_OBJ:%.o=%.shared.o)
	$(CC) -o $@ $(OS_LINK_FLAGS) $^ $(PROXYSOCKET_SHARED_LDFLAGS) $(PROXYSOCKET_LDFLAGS) $(LDFLAGS) $(LIBS)

examples: $(EXAMPLES_BIN)

%$(BINEXT): examples/%.static.o $(LIBPREFIX)proxysocket$(LIBEXT)
	$(CC) -o $@ examples/$(@:%$(BINEXT)=%.static.o) $(LIBPREFIX)proxysocket$(LIBEXT) $(PROXYSOCKET_LDFLAGS) $(LDFLAGS)

#%$(BINEXT): examples/%.shared.o $(LIBPREFIX)proxysocket$(LIBEXT)
#	$(CC) -o $@ examples/$(@:%$(BINEXT)=%.shared.o) $(LIBPREFIX)proxysocket$(LIBEXT) $(PROXYSOCKET_LDFLAGS) $(LDFLAGS)

tools: $(TOOLS_BIN)

.PHONY: doc
doc:
ifdef DOXYGEN
	$(DOXYGEN) doc/Doxyfile
endif

install: all doc
	$(MKDIR) $(PREFIX)/include $(PREFIX)/lib $(PREFIX)/bin
	$(CP) src/*.h $(PREFIX)/include/
	$(CP) *$(LIBEXT) $(PREFIX)/lib/
ifeq ($(OS),Windows_NT)
	$(CP) *$(SOEXT) $(PREFIX)/bin/
else
	$(CP) *$(SOEXT) $(PREFIX)/lib/
endif
	$(CP) $(TOOLS_BIN) $(PREFIX)/bin/
ifdef DOXYGEN
	$(CPDIR) doc/man $(PREFIX)/
endif

.PHONY: version
version:
	sed -ne "s/^#define\s*PROXYSOCKET_VERSION_[A-Z]*\s*\([0-9]*\)\s*$$/\1./p" src/proxysocket.h | tr -d "\n" | sed -e "s/\.$$//" > version

.PHONY: package
package: version
	tar cfJ proxysocket-$(shell cat version).tar.xz --transform="s?^?proxysocket-$(shell cat version)/?" $(SOURCE_PACKAGE_FILES)

.PHONY: package
binarypackage: version
	$(MAKE) PREFIX=binarypackage_temp install
	tar cfJ "proxysocket-$(shell cat version)-$(OS).tar.xz" --transform="s?^binarypackage_temp/??" $(COMMON_PACKAGE_FILES) binarypackage_temp/*
	rm -rf binarypackage_temp

.PHONY: clean
clean:
	$(RM) src/*.o examples/*.o *$(LIBEXT) *$(SOEXT) $(TOOLS_BIN) $(EXAMPLES_BIN) version proxysocket-*.tar.xz doc/doxygen_sqlite3.db
	$(RMDIR) doc/html doc/man

