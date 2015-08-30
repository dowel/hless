TARGET = hless
SRC =  main hless file buffer log screen plain_file cache input brush line error
SRC += meta_line chunk status_bar editbox interruptible progress border asynchronous
SRC += messagebox config mark

O = obj
S = src

CXX = clang++
CXXFLAGS = -std=c++0x -Wall -g -DNCURSES_OPAQUE=0
LIBS = -pthread -lncurses -lboost_regex
LDFLAGS = -rdynamic $(LIBS)

_TARGET = $(TARGET)
_OBJS = $(patsubst %,$O/%.o,$(SRC))
_DEPS = $(patsubst %,$O/%.o.d,$(SRC))

ifeq ($(V),1)
	quite = 
else
	quite = @
endif

all: $(_TARGET)

$(_TARGET): $(_OBJS)
	$(quite)echo [LD] $(_TARGET)
	$(quite)$(CXX) $(_OBJS) $(LDFLAGS) -o $(_TARGET)

$O/%.o: $S/%.cpp
	$(quite)echo [CXX] $@
	$(quite)mkdir -p obj
	$(quite)$(CXX) $(CXXFLAGS) -MM -MF $@.d -MT $@ $<
	$(quite)$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(quite)echo [CLEAN]
	$(quite)rm -f $(TARGET) $(_TARGET) $O/*

call: clean all

-include $(_DEPS)

