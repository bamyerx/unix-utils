CC=gcc
CFLAGS=-std=c23 -Wall -Wextra -Wpedantic -Wconversion -g -fsanitize=address,undefined
CPPFLAGS=-I$(CURDIR)/include
