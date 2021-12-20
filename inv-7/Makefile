#---- Final Binary
BINARY  := app

#---- Compiler
CC      := gcc

#-------------------------------------------------------------------------------

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

.SILENT:
all: main.c
	$(CC) $^ -o $(BINARY)

clean:
	@rm -f app
	@rm -f *.o
