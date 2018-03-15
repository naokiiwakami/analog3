PROJECT_ROOT = $(abspath ../..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party
PROTO = $(PROJECT_ROOT)/proto
COMPONENT_ROOT = $(abspath .)
UNAMES = $(shell uname -s)

# -include local.mk

TARGET_LIB_NAME = liba3api

TARGETS = synthserv.pb.cc $(TARGET_LIB_NAME).a

INCLUDES = -I$(PROJECT_ROOT)/src -I$(THIRD_PARTY)/include
CC = gcc
CPP = g++
LD  = g++
CPPLINT = $(PROJECT_ROOT)/third-party/bin/cpplint.py
CFLAGS = -I../nanopb $(INCLUDES)
CPPWARNFLAGS = -Wformat -Wpointer-arith -Wall -Werror=return-type -Wno-deprecated-declarations
CPPFLAGS = -std=gnu++11 -g $(CPPWARNFLAGS) $(INCLUDES)
LDFLAGS = -L$(THIRD_PARTY)/lib -L$(PROJECT_ROOT)/src/api
# LIB = -la3proto -lprotobuf -lreadline -lhistory -lcurses -lpthread -llog4cplus
# LIB += -lprotobuf
DEPFLAGS = -M

OBJS = \
	module.o \
	net_utils.o \
	synthserv.pb.o \
	synth_node.o \
	synth_service.o

all:
	make $(TARGETS)
	make -w -C $(COMPONENT_ROOT)/test

synthserv.pb.cc: $(PROTO)/synthserv.proto
	protoc -I$(PROTO) --cpp_out=. $(PROTO)/synthserv.proto

$(TARGET_LIB_NAME).a: $(OBJS)
	ar -r $(TARGET_LIB_NAME).a $(OBJS)

-include $(OBJS:.o=.d)

%.o: %.cc
	if [ -z "$(shell echo $* | grep '.pb')" ]; then $(CPPLINT) --quiet --filter=-legal/copyright --linelength=120 $*.h; fi
	if [ -z "$(shell echo $* | grep '.pb')" ]; then $(CPPLINT) --quiet --filter=-legal/copyright --linelength=120 $*.cc; fi
	$(CPP) -c -o $*.o $(CPPFLAGS) $*.cc
	$(CPP) $(DEPFLAGS) $(CPPFLAGS) $*.cc > $*.d

CLEANFILES = *.o *.d *~ core* $(TARGET) *.a synthserv.pb.cc synthserv.pb.h
CLEANESTFILES = $(CLEANFILES)

clean:
	rm -rf $(CLEANFILES)
#	make -C test clean

cleanest:
	rm -rf $(CLEANESTFILES)
#	make -C test cleanest
