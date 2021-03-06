UNAMES = $(shell uname -s)

# -include local.mk

TARGET = netsynth

CLEANFILES = *.o *~ core* $(TARGET)
CLEANESTFILES = $(CLEANFILES) *.d depend *.pb.* *_pb2.* *.pb

CC = gcc
CPP = g++
LD  = g++
CFLAGS = -I../nanopb
CPPFLAGS = -g -D_Linux_ -Wlong-long -Wformat -Wpointer-arith -Wall -I../thirdparty/include -I../nanopb
LDFLAGS = -L../thirdparty/lib
LIB = -lpthread -lprotobuf -llog4cplus
DEPFLAGS = -M

OBJS = \
	compact_descriptor.pb.o \
	module_descriptor.pb.o \
	connector.pb.o \
	main.o \
	nano_compact_descriptor.pb.o \
	pb_common.o \
	pb_encode.o \
	pb_decode.o \
	Component.o \
	FileModuleDriver.o \
	I2cModuleDriver.o \
	Listener.o \
	ModuleDriver.o \
	Session.o \
	Sock.o \
	Thread.o

all: $(TARGET) connector_pb2.py

netsynth: $(OBJS)
	$(LD) $(LDFLAGS) -o netsynth $(OBJS) $(LIB)

-include $(OBJS:.o=.d)

connector.pb.cc: ../protobuf/connector.proto module_descriptor.pb.cc
	protoc -I../protobuf --cpp_out=. ../protobuf/connector.proto

module_descriptor.pb.cc: ../protobuf/module_descriptor.proto
	protoc -I../protobuf --cpp_out=. ../protobuf/module_descriptor.proto

compact_descriptor.pb.cc: ../protobuf/compact_descriptor.proto
	protoc -I../protobuf --cpp_out=. ../protobuf/compact_descriptor.proto

nano_compact_descriptor.pb.c : ../protobuf/compact_descriptor.proto
	protoc -I../protobuf -o nano_compact_descriptor.pb ../protobuf/compact_descriptor.proto
	python ../nanopb/generator/nanopb_generator.py nano_compact_descriptor.pb

# compile and generate dependency info;
# more complicated dependency computation, so all prereqs listed
# will also become command-less, prereq-less targets
#   sed:    strip the target (everything before colon)
#   sed:    remove any continuation backslashes
#   fmt -1: list words one per line
#   sed:    strip leading spaces
#   sed:    add trailing colons
%.o: %.cpp
	$(CPP) -c -o $*.o $(CPPFLAGS) $*.cpp
	$(CPP) $(DEPFLAGS) $(CPPFLAGS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

pb_common.o: ../nanopb/pb_common.c
	$(CC) -c -o pb_common.o $(CFLAGS) ../nanopb/pb_common.c

pb_encode.o: ../nanopb/pb_encode.c
	$(CC) -c -o pb_encode.o $(CFLAGS) ../nanopb/pb_encode.c

pb_decode.o: ../nanopb/pb_decode.c
	$(CC) -c -o pb_decode.o $(CFLAGS) ../nanopb/pb_decode.c

connector_pb2.py: ../protobuf/connector.proto module_descriptor_pb2.py
	protoc -I../protobuf --python_out=. ../protobuf/connector.proto

module_descriptor_pb2.py: ../protobuf/module_descriptor.proto
	protoc -I../protobuf --python_out=. ../protobuf/module_descriptor.proto

clean:
	rm -rf $(CLEANFILES)

cleanest:
	rm -rf $(CLEANESTFILES)

# include depend
