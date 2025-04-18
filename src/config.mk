VERSION = 0.4.0

CC = cc

STD =

CFLAGS = ${STD} -Wall -fmax-errors=10 -Wextra -Werror -Wpedantic -march=native

DEBUG = ${STD} -Wall -fmax-errors=10 -Wextra -Werror -Wpedantic -g -O0

#libs required
LIBS = -lX11 -lXtst

#Paths
PERFIX = /usr/local
MANPERFIX = ${PERFIX}/share/man