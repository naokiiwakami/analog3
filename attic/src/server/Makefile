PROJECT_ROOT = $(abspath ../..)
COMPONENT_ROOT = $(abspath .)
-include $(PROJECT_ROOT)/project.mk

TARGET = synth

OBJS = \
	errors.o \
	event_handler.o \
	finder.o \
	node_builder.o \
	server.o \
	synth.o

all:
	make $(TARGET)
	make -w -C $(COMPONENT_ROOT)/test

libsynth.a: $(OBJS)
	ar -r libsynth.a $(OBJS)

synth: main.o libsynth.a $(API)/liba3api.a
	$(LD) $(LDFLAGS) -o synth main.o libsynth.a $(APP_LIBS)

-include $(OBJS:.o=.d)

clean:
	rm -rf $(CLEANFILES)
	make -C test clean

cleanest:
	rm -rf $(CLEANESTFILES)
	make -C test cleanest
