#mzfs Makefile
#Stephen Marz
#30 June 2016

PROG = makeblock
#Builder settable options
OBJ_PATH := objs
DEP_PATH := deps
INC_PATH := ./include

CXX	 := g++

LDFLAGS	 :=

CXXFLAGS := -MD -MP
CXXFLAGS += -O2 -Wall -g
CXXFLAGS += -std=c++11
CXXFLAGS += -I..

MV       ?= mv
RM       ?= rm

LIBS	 :=

#Only touch the options below if there are problems
SOURCES_CPP := $(wildcard *.cpp)

DEPS	:= $(wildcard $(DEP_PATH)/*.d)
OBJS	:= $(patsubst %.cpp,$(OBJ_PATH)/%.o,$(SOURCES_CPP))

default:
	@if [ ! -d $(DEP_PATH) ]; then mkdir $(DEP_PATH); fi
	@if [ ! -d $(OBJ_PATH) ]; then mkdir $(OBJ_PATH); fi
	@$(MAKE) $(PROG)

$(PROG): $(OBJS) Makefile
	@echo "  LINK $@"
	@$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

include $(DEPS)

$(OBJ_PATH)/%.o: %.cpp Makefile
	@echo "   CXX $< -> $@"
	@$(CXX) $(CXXFLAGS) -o $@ -c $<
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
	@$(RM) -f $(PROG)
