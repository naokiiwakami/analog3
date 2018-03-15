PROJECT_ROOT = $(abspath ../../..)
COMPONENT_ROOT = $(abspath ..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party

GTEST_LIB = $(THIRD_PARTY)/lib/gtest_main.a

CC = gcc
CXX = g++
LD = g++
CPPLINT = $(PROJECT_ROOT)/third-party/bin/cpplint.py
DEPFLAGS = -M
CPPFLAGS = -std=gnu++11 -O0 -g -I$(PROJECT_ROOT)/src -I$(THIRD_PARTY)/include
LDFLAGS = -L$(PROJECT_ROOT)/src/api -L$(THIRD_PARTY)/lib
LIBS = -la3api -lprotobuf $(GTEST_LIB) -lpthread -llog4cplus

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
	$(LD) -o $(SYNTH_NODE_TEST) $(LDFLAGS) $(SYNTH_NODE_TEST).o $(LIBS)
# end synth_node test

-include $(OBJS:.o=.d)
%.o: %.cc
	$(CPPLINT) --quiet --filter=-legal/copyright --linelength=120 $*.cc
	$(CXX) -c -o $*.o $(CPPFLAGS) $*.cc
	$(CXX) $(DEPFLAGS) $(CPPFLAGS) $*.cc > $*.d
#	@mv -f $*.d $*.d.tmp
#	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
#	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
#		sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
#	@rm -f $*.d.tmp

clean:
	rm -f *.o *.a *.d *.T $(PROGRAMS) core

cleanest:
	rm -f *.o *.a *.d *.T $(PROGRAMS) core
