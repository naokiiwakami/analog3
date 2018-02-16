PROJECT_ROOT = $(abspath ..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party
COMPONENT_ROOT = $(abspath .)
UNAMES = $(shell uname -s)

# -include local.mk

TARGET = synth

INCLUDES = -I../thirdparty/include -I../nanopb -I$(PROJECT_ROOT) -I$(THIRD_PARTY)/include
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

OBJS = \
	errors.o \
	event_handler.o \
	finder.o \
	module.o \
	node_builder.o \
	server.o \
	synth.o \
	synth_node.o \
	synthserv.pb.o

all:
	make $(TARGET)
	make -w -C $(COMPONENT_ROOT)/test

libsynth.a: $(OBJS)
	ar -r libsynth.a $(OBJS)

synth: main.o libsynth.a
	$(LD) $(LDFLAGS) -o synth main.o libsynth.a $(LIB)

-include $(OBJS:.o=.d)

%.o: %.cc
	if [ -z `echo $*.cc | grep '\.pb\.'` ]; then $(CPPLINT) --quiet --filter=-legal/copyright --linelength=120 $*.cc; fi
	$(CPP) -c -o $*.o $(CPPFLAGS) $*.cc
	$(CPP) $(DEPFLAGS) $(CPPFLAGS) $*.cc > $*.d.tmp
	sed -E "s#($(COMPONENT_ROOT)/\S+\.)h #\1hl #g" $*.d.tmp > $*.d
	rm $*.d.tmp

%.hl: %.h
	if [ -z `echo $*.h | grep '\.pb\.'` ]; then $(CPPLINT) --quiet --filter=-legal/copyright --linelength=120 $*.h; fi \
	&& touch $*.hl

synthserv.pb.cc: $(PROJECT_ROOT)/protocol/synthserv.proto
	protoc -I$(PROJECT_ROOT)/protocol --cpp_out=. $(PROJECT_ROOT)/protocol/synthserv.proto

CLEANFILES = *.o *.d *~ core* $(TARGET) *.a *.hl
CLEANESTFILES = $(CLEANFILES) depend *.pb.* *_pb2.* *.pb

clean:
	rm -rf $(CLEANFILES)
	make -C test clean

cleanest:
	rm -rf $(CLEANESTFILES)
	make -C test cleanest
