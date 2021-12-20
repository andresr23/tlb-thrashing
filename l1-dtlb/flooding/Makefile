#---- Final Binary
BINARY  := app

#---- Include
INCLUDE	:= -I../../

#---- Libraries
LDFLAGS	:= -L../../libtlb
LIBS    := -ltlb -lrt
LIBDIRS := ../../libtlb

#---- Flags
CFLAGS	:= $(INCLUDE)

#---- Compiler
CC      := gcc

#-------------------------------------------------------------------------------

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

BUILDDIRS = $(LIBDIRS:%=build-%)
CLEANDIRS = $(LIBDIRS:%=clean-%)

.PHONY: all clean $(BUILDDIRS) $(CLEANDIRS)

.SILENT:
all: $(BUILDDIRS)
	echo "============================== [${BINARY}]"
	make $(BINARY)

$(BINARY): $(OBJECTS)
	echo "  [LD]" $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BINARY) $(LDFLAGS) $(LIBS)

%.o: %.c
	echo "  [CC]" $<
	$(CC) $^ $(CFLAGS) -c

clean: $(CLEANDIRS)
	@rm -f app
	@rm -f *.o

$(BUILDDIRS):
	echo "============================== [${@:build-%=%}]"
	$(MAKE) -C $(@:build-%=%)

$(CLEANDIRS):
	$(MAKE) clean -C $(@:clean-%=%)
