#---Definitions---#
OUT_DIR=./out
OUT_PFX=$(OUT_DIR)/$(PFX)-

OBJ_DIR=./obj
CFLAGS=-Wall -Werror -I./include
CXXFLAGS=$(CFLAGS) -std=c++11

LIBRARY_OBJS=$(OBJ_DIR)/src/cothread.o $(OBJ_DIR)/src/$(PFX)/cothread.o

#---Suppress display of executed commands---#
.SILENT:

#---First rule---#
all:	init unittest tuto0 tuto1

#---Initialization rule---#
init:
	cmake -E remove_directory $(OBJ_DIR)

	cmake -E make_directory $(OUT_DIR)
	cmake -E make_directory $(OBJ_DIR)/src/$(PFX)
	cmake -E make_directory $(OBJ_DIR)/unittest/src
	cmake -E make_directory $(OBJ_DIR)/tuto0
	cmake -E make_directory $(OBJ_DIR)/tuto1

#---Global rules---#
$(OBJ_DIR)/%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $^

$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJ_DIR)/%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $^

#---Unittest rules---#
unittest: CFLAGS+=-I./unittest/include
unittest:										\
	$(LIBRARY_OBJS)								\
	$(OBJ_DIR)/unittest/src/unittest0.o			\
	$(OBJ_DIR)/unittest/src/unittest1.o			\
	$(OBJ_DIR)/unittest/src/unittest2.o			\
	$(OBJ_DIR)/unittest/src/unittest3.o			\
	$(OBJ_DIR)/unittest/src/unittest_cothread.o	\
	$(OBJ_DIR)/unittest/src/main.o
	$(CXX) -o $(OUT_PFX)$@ $^

#---Tuto0 rules---#
tuto0:	$(LIBRARY_OBJS) $(OBJ_DIR)/tuto0/main.o
	$(CC) -o $(OUT_PFX)$@ $^

#---Tuto1 rules---#
tuto1:	CFLAGS+=-I./tuto1
tuto1:							\
	$(LIBRARY_OBJS)				\
	$(OBJ_DIR)/tuto1/stream.o	\
	$(OBJ_DIR)/tuto1/parser0.o	\
	$(OBJ_DIR)/tuto1/parser1.o	\
	$(OBJ_DIR)/tuto1/main.o
	$(CC) -o $(OUT_PFX)$@ $^
