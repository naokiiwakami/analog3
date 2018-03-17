PROJECT_ROOT = $(abspath ../..)
COMPONENT_ROOT = $(abspath .)
-include $(PROJECT_ROOT)/project.mk

TARGET_LIB_NAME = liba3api

TARGETS = synthserv.pb.cc $(TARGET_LIB_NAME).a

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

clean:
	rm -rf $(CLEANFILES)
	make -C test clean

cleanest:
	rm -rf $(CLEANESTFILES)
#	make -C test cleanest
