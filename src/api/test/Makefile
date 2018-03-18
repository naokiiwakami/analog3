PROJECT_ROOT = $(abspath ../../..)
COMPONENT_ROOT = $(abspath ..)
-include $(PROJECT_ROOT)/project.mk

# TEST_LIBS = -la3api $(LIBS)

all:
	make $(TARGETS)

# synth_node test
SYNTH_NODE_TEST = synth_node_test
TARGETS += $(SYNTH_NODE_TEST).T
PROGRAMS += $(SYNTH_NODE_TEST)
OBJS += $(SYNTH_NODE_TEST).o
$(SYNTH_NODE_TEST).T: $(SYNTH_NODE_TEST)
	$(VALGRIND) ./synth_node_test && touch synth_node_test.T
$(SYNTH_NODE_TEST): $(SYNTH_NODE_TEST).o $(PROJECT_ROOT)/src/api/liba3api.a
	rm -f $(SYNTH_NODE_TEST)
	$(LD) -o $(SYNTH_NODE_TEST) $(LDFLAGS) $(SYNTH_NODE_TEST).o $(APP_LIBS)
# end synth_node test

-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.a *.d *.T $(PROGRAMS) core

cleanest:
	rm -f *.o *.a *.d *.T $(PROGRAMS) core
