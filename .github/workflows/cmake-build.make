CFLAGS=-Wall -Werror -I$(CMAKE_INSTALL_PREFIX)/include/cothread-1.0
CXXFLAGS=$(CFLAGS) -std=c++11

.SILENT:
all:	unittest

unittest: CFLAGS+=-I$(CMAKE_SOURCE_DIR)/unittest/include
unittest:											\
	$(CMAKE_SOURCE_DIR)/unittest/src/unittest0.o	\
	$(CMAKE_SOURCE_DIR)/unittest/src/unittest1.o	\
	$(CMAKE_SOURCE_DIR)/unittest/src/unittest2.o	\
	$(CMAKE_SOURCE_DIR)/unittest/src/main.o
	$(CXX)											\
	-o $(CMAKE_INSTALL_PREFIX)/bin/$@				\
	-L $(CMAKE_INSTALL_PREFIX)/lib					\
	-Wl,-rpath,$(CMAKE_INSTALL_PREFIX)/lib			\
	$^ -lcothread

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^
