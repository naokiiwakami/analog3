PROJECT_ROOT = $(abspath ../../..)
COMPONENT_ROOT = $(abspath ..)
-include $(PROJECT_ROOT)/project.mk

TEST_LIBS = $(COMPONENT_ROOT)/libsynth.a $(APP_LIBS)

all:
	make $(TARGETS)

# types test
MODULE_TEST = module_test
TARGETS += $(MODULE_TEST).T
PROGRAMS += $(MODULE_TEST)
OBJS += $(MODULE_TEST).o
$(MODULE_TEST).T: $(MODULE_TEST)
	$(VALGRIND) ./module_test && touch module_test.T
$(MODULE_TEST): $(SERVER_LIB) $(MODULE_TEST).o
	rm -f $(MODULE_TEST)
	$(LD) -o $(MODULE_TEST) $(LDFLAGS) $(MODULE_TEST).o $(TEST_LIBS)
# end module test

-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.a *.d *.T $(PROGRAMS) core

cleanest:
	rm -f *.o *.a *.d *.T $(PROGRAMS) core
