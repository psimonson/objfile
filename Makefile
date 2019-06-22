CFLAGS+=-std=c89 -Wall -Wextra -Werror -g $(shell pkg-config --cflags prs)
LDFLAGS+=$(shell pkg-config --libs prs) -lGL -lGLU -lglut

SRCDIR=$(shell pwd)

VERSION=0.1
BASENAM=$(shell echo $$(basename $(SRCDIR)))
TARNAME=$(BASENAM)-$(VERSION).tgz

SOURCE=$(shell find . -maxdepth 1 -iname '*.c')
OBJECT=$(SOURCE:%.c=%.c.o)
TARGET=objfile

.PHONY: all clean
all: $(TARGET)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $(OBJECT) $(LDFLAGS)

clean:
	rm -f *~ *.log $(OBJECT) $(TARGET)

distclean: clean

dist:
	@echo Compressing $(TARNAME)...
	@cd .. && tar --exclude=.git -cf - ./$(BASNAME) | \
	gzip -9 > $(TARNAME) && echo Compression done! || \
	echo Compression failed. || exit 1
