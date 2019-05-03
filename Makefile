# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC = dwm_status.c

all: options dwm_status

options:
	@echo dwm build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"


dwm_status: dwm_status.c 
	${CC} -o $@ ${SRC} ${LDFLAGS}

clean:
	rm -f dwm_status ${OBJ} 

install: all
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f dwm_status "$(DESTDIR)$(PREFIX)/bin"
	chmod 755 "$(DESTDIR)$(PREFIX)/bin/dwm_status"

.PHONY: all options clean 
