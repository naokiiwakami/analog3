UNAMES = $(shell uname -s)

# -include local.mk

TARGET = netsynth

CLEANFILES = *.o *~ core* $(TARGET)
CLEANESTFILES = $(CLEANFILES) *.d depend *.pb.* *_pb2.*

CPP = g++
LD  = g++
CFLAGS = -I../nanopb
CPPFLAGS = -g -D_Linux_ -Wlong-long -Wformat -Wpointer-arith -Wall -I../thirdparty/include -I../nanopb
LDFLAGS = -L../thirdparty/lib 
LIB = -lpthread -lprotobuf -llog4cplus
DEPFLAGS = -M

OBJS = \
	connector.pb.o \
	main.o \
	nano_connector.pb.o \
	pb_common.o \
	pb_encode.o \
	pb_decode.o \
	FileModuleDriver.o \
	Listener.o \
	ModuleDriver.o \
	Session.o \
	Sock.o \
	Thread.o

all: $(TARGET) connector_pb2.py

netsynth: $(OBJS)
	$(LD) $(LDFLAGS) -o netsynth $(OBJS) $(LIB)

-include $(OBJS:.o=.d)

connector.pb.cc: ../protobuf/connector.proto
	protoc -I../protobuf --cpp_out=. ../protobuf/connector.proto

nano_connector.pb.c : ../protobuf/connector.proto
	protoc -I../protobuf -o nano_connector.pb ../protobuf/connector.proto
	python ../nanopb/generator/nanopb_generator.py nano_connector.pb

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

connector_pb2.py: ../protobuf/connector.proto
	protoc -I../protobuf --python_out=. ../protobuf/connector.proto

clean:
	rm -rf $(CLEANFILES)

cleanest:
	rm -rf $(CLEANESTFILES)

# include depend
