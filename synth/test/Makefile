PROJECT_ROOT = $(abspath ../..)
COMPONENT_ROOT = $(abspath ..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party

GTEST_LIB = $(THIRD_PARTY)/lib/gtest_main.a

COMMON_LIB = $(COMPONENT_ROOT)/libsynth.a

CC = gcc
CXX = g++
LD = g++
CPPLINT = $(PROJECT_ROOT)/third-party/bin/cpplint.py
DEPFLAGS = -M
CPPFLAGS = -std=gnu++11 -O0 -g -I$(PROJECT_ROOT) -I$(THIRD_PARTY)/include
LDFLAGS = -L$(THIRD_PARTY)/lib
LIBS = $(COMMON_LIB) $(GTEST_LIB) -lpthread -llog4cplus

all:
	make $(TARGETS)

# types test
MODULE_TEST = module_test
TARGETS += $(MODULE_TEST).T
PROGRAMS += $(MODULE_TEST)
OBJS += $(MODULE_TEST).o
$(MODULE_TEST).T: $(MODULE_TEST)
	$(VALGRIND) ./module_test && touch module_test.T
$(MODULE_TEST): $(COMMON_LIB) $(MODULE_TEST).o
	rm -f $(MODULE_TEST)
	$(LD) -o $(MODULE_TEST) $(LDFLAGS) $(MODULE_TEST).o $(LIBS)
# end module test

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
