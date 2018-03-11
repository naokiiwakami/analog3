PROJECT_ROOT = $(abspath ../..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party
COMPONENT_ROOT = $(abspath .)

TARGET = liba3proto.a
all: $(TARGET)

INCLUDES = -I../thirdparty/include -I../nanopb -I$(PROJECT_ROOT)/src -I$(THIRD_PARTY)/include
CPP = g++
LD  = g++
CPPWARNFLAGS = -Wformat -Wpointer-arith -Wall -Werror=return-type
CPPFLAGS = -std=gnu++11 -g $(CPPWARNFLAGS) $(INCLUDES)

# synthserv.pb.cc: synthserv.proto
#	protoc -I. --cpp_out=. ./synthserv.proto

OBJS = \
	synthserv.pb.o

$(TARGET): $(OBJS)
	ar -r $(TARGET) $(OBJS)

%.pb.cc: %.proto
	protoc -I. --cpp_out=. $*.proto

%.pb.o: %.pb.cc
	$(CPP) -c -o $*.pb.o $(CPPFLAGS) $*.pb.cc

clean:
	rm -rf *.cc *.h *.o $(TARGET)
