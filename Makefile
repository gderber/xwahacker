CC=gcc
CROSS_CC=i686-w64-mingw32-gcc
CFLAGS=-Wall -Wdeclaration-after-statement -Wpointer-arith -Wredundant-decls -Wcast-qual -Wwrite-strings -g -Os
CFLAGS+=-std=c99 -D_XOPEN_SOURCE=500
LDFLAGS=-lm

all: xwahacker.exe

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.exe: %.c
	$(CROSS_CC) -static $(CFLAGS) $(LDFLAGS) $^ -o $@

release: xwahacker.zip

xwahacker.zip: *.bat xwahacker.exe readme.txt LICENSE xwahacker.c
	strip xwahacker.exe
	7z a -mx=9 $@ $^

upload: xwahacker.zip readme.txt
	scp $^ $(SFUSER),xwahacker@frs.sourceforge.net:/home/frs/project/x/xw/xwahacker

clean:
	rm -rf xwahacker xwahacker.exe xwahacker.zip

.PHONY: all clean release upload
