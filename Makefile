TARGET = hless
SRC =  main hless file buffer log screen plain_file cache input brush line error
SRC += meta_line chunk status_bar editbox interruptible progress border asynchronous
SRC += messagebox

O = obj
S = src

CXX = g++
CXXFLAGS = -Wall -g
LIBS = -pthread -lncurses -lboost_regex-mt
LDFLAGS = -rdynamic $(LIBS)

_TARGET = $(TARGET)
_OBJS = $(patsubst %,$O/%.o,$(SRC))
_DEPS = $(patsubst %,$O/%.o.d,$(SRC))

all: $(_TARGET)

$(_TARGET): $(_OBJS)
	@echo [LD] $(_TARGET)
	@$(CXX) $(_OBJS) $(LDFLAGS) -o $(_TARGET)

$O/%.o: $S/%.cpp
	@echo [CXX] $@
	@mkdir -p obj
	@$(CXX) $(CXXFLAGS) -MM -MD -MF $@.d -MT $@ $<
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo [CLEAN]
	@rm -f $(TARGET) $(_TARGET) $O/*

call: clean all

-include $(_DEPS)

