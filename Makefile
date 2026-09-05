.POSIX:

CC       = cc
CFLAGS   = -std=c17 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined
CPPFLAGS = -D_POSIX_C_SOURCE=202405L -I$(CURDIR)/include

PROGRAMS = echo cat wc head tail
TARGETS = $(PROGRAMS:%=build/%)

.PHONY: all clean

all: $(TARGETS)

build:
	mkdir -p $@
build/cat: src/cat/cat.c build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $<
build/echo: src/echo/echo.c build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $<
build/wc: src/wc/wc.c build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $<
build/head: src/head/head.c build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $<
build/tail: src/tail/tail.c build
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf build
