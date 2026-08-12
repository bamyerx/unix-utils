CC= cc
CFLAGS= -std=c17  -Wall -Wextra -Wpedantic -Wconversion -g -fsanitize=address,undefined
CPPFLAGS= -D_POSIX_C_SOURCE=202405L -I$(CURDIR)/include
