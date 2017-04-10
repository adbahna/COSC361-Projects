#mzfs Makefile
#Stephen Marz
#30 June 2016

PROG = fs
#Builder settable options
OBJ_PATH := objs
DEP_PATH := deps
MNT_PATH := mnt
INC_PATH := ./include

CXX	 := g++

LDFLAGS	 :=

CXXFLAGS := -MD -MP
CXXFLAGS += -O2 -Wall -Wno-write-strings -g
CXXFLAGS += -std=c++11
CXXFLAGS += -D_FILE_OFFSET_BITS=64
CXXFLAGS += -DFUSE_USE_VERSION=29
CXXFLAGS += -I. -I/home/smarz1/Programs/include/fuse

MV       ?= mv
RM       ?= rm

LIBS	 := /home/smarz1/Programs/lib/libfuse.a -lpthread -ldl

#Only touch the options below if there are problems
SOURCES_CPP := $(wildcard *.cpp)

DEPS	:= $(wildcard $(DEP_PATH)/*.d)
OBJS	:= $(patsubst %.cpp,$(OBJ_PATH)/%.o,$(SOURCES_CPP))

all:
	@if [ ! -d $(MNT_PATH) ]; then mkdir $(MNT_PATH); fi
	@if [ ! -d $(DEP_PATH) ]; then mkdir $(DEP_PATH); fi
	@if [ ! -d $(OBJ_PATH) ]; then mkdir $(OBJ_PATH); fi
	@$(MAKE) $(PROG)

$(PROG): $(OBJS) Makefile
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

include $(DEPS)

$(OBJ_PATH)/%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ -c $<
	@$(MV) $(OBJ_PATH)/$(patsubst %.cpp,%.d,$<) $(DEP_PATH)/$(patsubst %.cpp,%.d,$<)

.PHONY: clean distclean perm run

oclean:
	@echo "  CLEAN"
	@$(RM) -f $(DEPS)
	@$(RM) -f $(OBJS)
	@$(RM) -f $(PROG)

clean:
	@echo "  DISTCLEAN"
	@$(RM) -fr $(DEP_PATH)
	@$(RM) -fr $(OBJ_PATH)
	@$(RM) -fr $(MNT_PATH)
	@$(RM) -f $(PROG)
