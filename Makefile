# Simple makefile for gcc written by stext editor.
CC=gcc
CFLAGS=-std=c11 -W -O -g
CFLAGS+=-Ilibprs/include
LDFLAGS=-lglut -lGL -lGLU
LDFLAGS+=libprs/build/libprs_static.a

BACKUPS=$(shell find . -iname "*.bak")
SRCDIR=$(shell basename $(shell pwd))
DESTDIR?=
PREFIX?=usr
VERSION=1.0

SOURCE=$(wildcard *.c)
OBJECTS=$(SOURCE:%.c=%.c.o)
TARGET=objfile

.PHONY: all libprs install uninstall clean  distclean dist
all: $(TARGET)

libprs:
ifneq ($(test -d libprs),1)
	git clone https://github.com/psimonson/libprs.git
	cd libprs && ./build.sh
endif

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): libprs $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

install: all
	install $(TARGET) $(DESTDIR)/$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

distclean: clean
ifneq ($(test -d libprs),1)
	rm -rf libprs
endif
ifneq ($(BACKUPS),)
	rm -f *.bak
endif

dist: distclean
	cd .. && tar -cv --exclude=.git ./$(SRCDIR) | xz -9 > $(SRCDIR)-$(VERSION).tar.xz
