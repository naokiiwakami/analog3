# Project common makefile
#

THIRD_PARTY = $(PROJECT_ROOT)/third-party
PROTO = $(PROJECT_ROOT)/proto
API = $(PROJECT_ROOT)/src/api
UNAMES = $(shell uname -s)

GTEST_LIB = $(THIRD_PARTY)/lib/gtest_main.a

ifeq ($(USE_VALGRIND),y)
	VALGRIND = USE_VALGRIND=yes valgrind -q --leak-check=full --error-exitcode=1
else
	VALGRIND =
endif

INCLUDES = -I$(PROJECT_ROOT)/src -I$(THIRD_PARTY)/include
CC = gcc
CPP = g++
LD  = g++

CFLAGS = -I../nanopb $(INCLUDES)
# Temporary suppress warnings coming from third-party components
CPP_SUPPRESS_WARNINGS = -Wno-deprecated-declarations -Wno-sign-compare
CPPWARNFLAGS = -Wformat -Wpointer-arith -Wall -Werror=return-type
CPPFLAGS = -std=gnu++11 -g $(CPPWARNFLAGS) $(INCLUDES)

LDFLAGS = -L$(THIRD_PARTY)/lib -L$(PROJECT_ROOT)/src/api
LIBS = -lprotobuf $(GTEST_LIB) -lreadline -lhistory -lcurses -lpthread -llog4cplus
APP_LIBS = -la3api $(LIBS)
DEPFLAGS = -M

CPPLINT = $(PROJECT_ROOT)/third-party/bin/cpplint.py
CPPLINT_OPTIONS = --quiet --filter=-legal/copyright,-whitespace/parens --linelength=120

%.o: %.cc
	if [ -z "$(shell echo $* | grep '.pb')" ]; then $(CPPLINT) $(CPPLINT_OPTIONS) $*.h; fi
	if [ -z "$(shell echo $* | grep '.pb')" ]; then $(CPPLINT) $(CPPLINT_OPTIONS) $*.cc; fi
	$(CPP) -c -o $*.o $(CPPFLAGS) $*.cc
	$(CPP) $(DEPFLAGS) $(CPPFLAGS) $*.cc > $*.d

CLEANFILES = *.o *.d *~ core* $(TARGET) *.a *.hl
CLEANESTFILES = $(CLEANFILES)
