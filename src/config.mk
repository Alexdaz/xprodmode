VERSION = 0.1.0

CC = cc

STD =

CFLAGS = ${STD} -Wall -fmax-errors=10 -Wextra -pedantic -march=native

DEBUG = ${STD} -Wall -fmax-errors=10 -Wextra -pedantic -g -O0

#libs required
LIBS = -lX11 -lXtst

#Paths
PERFIX = /usr/local
MANPERFIX = ${PERFIX}/share/man