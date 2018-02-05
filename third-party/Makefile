PROJECT_ROOT = $(abspath ..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party

GTEST_DIR = $(THIRD_PARTY)/googletest
GTEST_MAKE = $(GTEST_DIR)/googletest/make
GTEST_LIB = $(GTEST_MAKE)/gtest_main.a
GTEST_INCLUDE = $GTEST_DIR)/googletest/include

STYLEGUIDE_DIR = $(THIRD_PARTY)/styleguide

RAPIDJSON = $(THIRD_PARTY)/rapidjson

LOG4CPLUS = $(THIRD_PARTY)/log4cplus-1.2.1

targets = googletest styleguide rapidjson log4cplus

all: $(targets)

###
googletest: .done-googletest
.done-googletest: $(GTEST_LIB) .done-installdirs
	rm -rf include/gtest && ln -s $(GTEST_DIR)/googletest/include/gtest include/gtest && cp $(GTEST_LIB) lib/ \
	&& touch .done-googletest
$(GTEST_LIB): .done-googletest-download
	make -C $(GTEST_MAKE)
.done-googletest-download:
	git clone git@github.com:google/googletest.git && touch .done-googletest-download

###
styleguide: .done-styleguide
.done-styleguide: .done-styleguide-download
	rm -f bin/cpplint.py && ln -s $(STYLEGUIDE_DIR)/cpplint/cpplint.py bin/cpplint.py && touch .done-styleguide
.done-styleguide-download:
	git clone git://github.com/google/styleguide.git && touch .done-styleguide-download

###
rapidjson: .done-rapidjson
.done-rapidjson: .done-rapidjson-download
	rm -rf include/rapidjson && ln -s $(RAPIDJSON)/include/rapidjson include/ && touch .done-rapidjson
.done-rapidjson-download:
	git clone git@github.com:Tencent/rapidjson.git && touch .done-rapidjson-download

###
log4cplus: .done-log4cplus
.done-log4cplus: .done-log4cplus-download
	rm -rf include/log4cplus lib/liblog4* lib/pkgconfig/log4cplus.pc && cd $(LOG4CPLUS) && \
	./configure --prefix=$(THIRD_PARTY) && make install && cd $(THIRD_PARTY) && touch .done-log4cplus
.done-log4cplus-download:
	wget https://sourceforge.net/projects/log4cplus/files/log4cplus-stable/1.2.1/log4cplus-1.2.1.tar.xz/download \
	-O log4cplus-1.2.1.tar.xz && tar xvf log4cplus-1.2.1.tar.xz && touch .done-log4cplus-download

###
.done-installdirs:
	mkdir -p bin && mkdir lib && mkdir include && touch .done-installdirs

clean:
	if [ -d "$(GTEST_DIR)" ]; then make -C $(GTEST_DIR)/googletest/make clean; fi
	if [ -d "$(LOG4CPLUS)" ]; then make -C $(LOG4CPLUS) clean; fi
	rm -f .done-log4cplus

cleanest:
	make clean
	rm -rf $(GTEST_DIR) $(STYLEGUIDE_DIR) $(LOG4CPLUS) log4cplus-1.2.1.tar.xz rapidjson bin include lib .done-*
