PROJECT_ROOT = $(abspath ..)
THIRD_PARTY = $(PROJECT_ROOT)/third-party
THIRD_PARTY_SRC = $(THIRD_PARTY)/src

GTEST_TOP = $(THIRD_PARTY_SRC)/googletest
GTEST_VER = release-1.8.0

GTEST_DIR = $(GTEST_TOP)/googletest
GTEST_MAKE = $(GTEST_DIR)/make
GTEST_LIB = gtest_main.a
GTEST_INCLUDE = $(GTEST_DIR)/include

GMOCK_DIR = $(GTEST_TOP)/googlemock
GMOCK_MAKE = $(GMOCK_DIR)/make
GMOCK_LIB = gmock_main.a
GMOCK_INCLUDE = $(GMOCK_DIR)/include

STYLEGUIDE_DIR = $(THIRD_PARTY_SRC)/styleguide

RAPIDJSON_DIR = $(THIRD_PARTY_SRC)/rapidjson
RAPIDJSON_VER = v1.1.0

LOG4CPLUS_REPOSITORY = https://sourceforge.net/projects/log4cplus/files/log4cplus-stable
LOG4CPLUS_VER = 1.2.1
LOG4CPLUS_DIR = $(THIRD_PARTY_SRC)/log4cplus-$(LOG4CPLUS_VER)

PROTOBUF_DIR = $(THIRD_PARTY_SRC)/protobuf
PROTOBUF_VER = v3.5.1.1

targets = googletest googlemock styleguide rapidjson log4cplus protobuf

all: $(targets)

###
googletest: .done-googletest
.done-googletest: $(GTEST_MAKE)/$(GTEST_LIB) .done-installdirs
	rm -rf include/gtest lib/$(GTEST_LIB)
	ln -s $(GTEST_DIR)/include/gtest include/gtest
	ln -s $(GTEST_MAKE)/$(GTEST_LIB) lib/
	touch .done-googletest
$(GTEST_MAKE)/$(GTEST_LIB): .done-googletest-download
	make -C $(GTEST_MAKE)

googlemock: .done-googlemock
.done-googlemock: $(GMOCK_MAKE)/$(GMOCK_LIB) .done-googletest
	rm -rf include/gmock lib/$(GMOCK_LIB)
	ln -s $(GMOCK_DIR)/include/gtest include/gmock
	ln -s $(GMOCK_MAKE)/$(GMOCK_LIB) lib/
	touch .done-googlemock
$(GMOCK_MAKE)/$(GMOCK_LIB): .done-googletest-download
	make -C $(GMOCK_MAKE)

.done-googletest-download: .done-installdirs
	rm -rf $(GTEST_TOP)
	git clone git@github.com:google/googletest.git $(GTEST_TOP)
	cd $(GTEST_TOP) && git checkout -b $(GTEST_VER) $(GTEST_VER)
	touch .done-googletest-download
###
styleguide: .done-styleguide
.done-styleguide: .done-styleguide-download
	rm -f bin/cpplint.py
	ln -s $(STYLEGUIDE_DIR)/cpplint/cpplint.py bin/cpplint.py
	touch .done-styleguide
.done-styleguide-download: .done-installdirs
	rm -rf $(STYLEGUIDE_DIR)
	git clone git://github.com/google/styleguide.git $(STYLEGUIDE_DIR)
	touch .done-styleguide-download

###
rapidjson: .done-rapidjson
.done-rapidjson: .done-rapidjson-download
	rm -rf include/rapidjson && ln -s $(RAPIDJSON_DIR)/include/rapidjson include/
	touch .done-rapidjson
.done-rapidjson-download: .done-installdirs
	rm -rf $(RAPIDJSON_DIR)
	git clone git@github.com:Tencent/rapidjson.git $(RAPIDJSON_DIR)
	cd $(RAPIDJSON_DIR) && git checkout -b $(RAPIDJSON_VER) $(RAPIDJSON_VER)
	touch .done-rapidjson-download

###
log4cplus: .done-log4cplus
.done-log4cplus: .done-log4cplus-download
	rm -rf include/log4cplus lib/liblog4* lib/pkgconfig/log4cplus.pc
	cd $(LOG4CPLUS_DIR) && ./configure --prefix=$(THIRD_PARTY) && make install
	touch .done-log4cplus
.done-log4cplus-download: .done-installdirs
	rm -rf src/log4cplus-$(LOG4CPLUS_VER).tar.xz src/log4cplus-$(LOG4CPLUS_VER)
	wget $(LOG4CPLUS_REPOSITORY)/$(LOG4CPLUS_VER)/log4cplus-$(LOG4CPLUS_VER).tar.xz/download \
		-O src/log4cplus-$(LOG4CPLUS_VER).tar.xz
	cd src && tar xvf log4cplus-$(LOG4CPLUS_VER).tar.xz
	touch .done-log4cplus-download

###
protobuf: .done-protobuf
.done-protobuf: .done-protobuf-download
	cd $(PROTOBUF_DIR) && ./autogen.sh && ./configure --prefix=$(THIRD_PARTY) && make install
	touch .done-protobuf
.done-protobuf-download: .done-installdirs
	rm -rf $(PROTOBUF_DIR)
	git clone git@github.com:google/protobuf.git $(PROTOBUF_DIR)
	cd $(PROTOBUF_DIR) && git checkout -b $(PROTOBUF_VER) $(PROTOBUF_VER)
	touch .done-protobuf-download

###
.done-installdirs:
	mkdir -p bin
	mkdir -p lib
	mkdir -p include
	mkdir -p src
	touch .done-installdirs

cleanest:
	rm -rf bin include lib src .done-*
