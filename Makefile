# Simple makefile for gcc written by stext editor.
CC=gcc
CFLAGS=-std=c11 -W -O -g
CFLAGS+=$(shell pkg-config --cflags prs)
LDFLAGS=-lglut -lGL -lGLU
LDFLAGS+=$(shell pkg-config --libs prs)

BACKUPS=$(shell find . -iname "*.bak")
SRCDIR=$(shell basename $(shell pwd))
DESTDIR?=
PREFIX?=usr
VERSION=1.0

SOURCE=$(wildcard *.c)
OBJECTS=$(SOURCE:%.c=%.c.o)
TARGET=objfile

.PHONY: all install uninstall clean  distclean dist
all: $(TARGET)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: all
	install $(TARGET) $(DESTDIR)/$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

distclean: clean
ifneq ($(BACKUPS),)
	rm -f *.bak
endif

dist: distclean
	cd .. && tar -cv --exclude=.git ./$(SRCDIR) | xz -9 > $(SRCDIR)-$(VERSION).tar.xz
