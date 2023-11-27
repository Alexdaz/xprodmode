include config.mk

xprodmode: xprodmode.c config.mk
	@echo "CC      =" ${CC}
	@echo "CFLAGS  =" ${CFLAGS}
	@echo "LIBS    =" ${LIBS}
	@echo "VERSION =" ${VERSION}

	${CC} -DVERSION=\"${VERSION}\" ${CFLAGS} -c xprodmode.c
	${CC} ${CFLAGS} xprodmode.o -o $@ ${LIBS}

debug: xprodmode.c config.mk
	@echo "CC      =" ${CC}
	@echo "DEBUG   =" ${DEBUG}
	@echo "LIBS    =" ${LIBS}
	@echo "VERSION =" ${VERSION}

	${CC} -DVERSION=\"${VERSION}\" ${DEBUG} -c xprodmode.c
	${CC} ${DEBUG} xprodmode.o -o $@ ${LIBS}

clean:
	rm -f *.o xprodmode debug

install:
	@echo installing to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	cp xprodmode ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/xprodmode
	cp ../man/xprodmode.1 /usr/share/man/man1/
	chmod 644 /usr/share/man/man1/xprodmode.1

uninstall:
	@rm -f ${PREFIX}/bin/xprodmode
	@rm -f /usr/share/man/man1/xprodmode.1
