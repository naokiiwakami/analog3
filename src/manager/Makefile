PROJECT_ROOT = $(abspath ../..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party
COMPONENT_ROOT = $(abspath .)
UNAMES = $(shell uname -s)

# -include local.mk

TARGET = manager

INCLUDES = -I../thirdparty/include -I../nanopb -I$(PROJECT_ROOT)/src -I$(THIRD_PARTY)/include
CC = gcc
CPP = g++
LD  = g++
CPPLINT = $(PROJECT_ROOT)/third-party/bin/cpplint.py
CFLAGS = -I../nanopb $(INCLUDES)
CPPWARNFLAGS = -Wformat -Wpointer-arith -Wall -Werror=return-type
CPPFLAGS = -std=gnu++11 -g $(CPPWARNFLAGS) $(INCLUDES)
LDFLAGS = -L$(THIRD_PARTY)/lib
LIB = -lpthread -llog4cplus
# LIB += -lprotobuf
DEPFLAGS = -M

OBJS =

all:
	make $(TARGET)
#	make -w -C $(COMPONENT_ROOT)/test

manager: main.o
	$(LD) $(LDFLAGS) -o manager main.o $(LIB)

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
#	make -C test clean

cleanest:
	rm -rf $(CLEANESTFILES)
#	make -C test cleanest
