PROJECT_ROOT = $(abspath ../..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party
COMPONENT_ROOT = $(abspath .)
API = $(abspath ../api)
UNAMES = $(shell uname -s)

# -include local.mk

TARGET = synth

INCLUDES = -I$(PROJECT_ROOT)/src -I$(THIRD_PARTY)/include
CC = gcc
CPP = g++
LD  = g++
CPPLINT = $(PROJECT_ROOT)/third-party/bin/cpplint.py
CFLAGS = -I../nanopb $(INCLUDES)
CPPWARNFLAGS = -Wformat -Wpointer-arith -Wall -Werror=return-type -Wno-deprecated-declarations
CPPFLAGS = -std=gnu++11 -g $(CPPWARNFLAGS) $(INCLUDES)
LDFLAGS = -L$(THIRD_PARTY)/lib -L$(PROJECT_ROOT)/src/protocol -L$(API)
LIB = -lpthread -llog4cplus -la3api -lprotobuf
# LIB += -lprotobuf
DEPFLAGS = -M

OBJS = \
	errors.o \
	event_handler.o \
	finder.o \
	module.o \
	node_builder.o \
	server.o \
	synth.o \
	synth_node.o

all:
	make $(TARGET)
	make -w -C $(COMPONENT_ROOT)/test

libsynth.a: $(OBJS)
	ar -r libsynth.a $(OBJS)

synth: main.o libsynth.a
	$(LD) $(LDFLAGS) -o synth main.o libsynth.a $(LIB)

-include $(OBJS:.o=.d)

%.o: %.cc
	$(CPPLINT) --quiet --filter=-legal/copyright --linelength=120 $*.h
	$(CPPLINT) --quiet --filter=-legal/copyright --linelength=120 $*.cc
	$(CPP) -c -o $*.o $(CPPFLAGS) $*.cc
	$(CPP) $(DEPFLAGS) $(CPPFLAGS) $*.cc > $*.d

CLEANFILES = *.o *.d *~ core* $(TARGET) *.a *.hl
CLEANESTFILES = $(CLEANFILES)

clean:
	rm -rf $(CLEANFILES)
	make -C test clean

cleanest:
	rm -rf $(CLEANESTFILES)
	make -C test cleanest
