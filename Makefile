TARGET = hless
SRC = main hless file buffer log screen plain_file cache input brush line error

O = obj
S = src

CXX = g++
CXXFLAGS = -Wall -g
LIBS = -pthread -lncurses

_TARGET = $(TARGET)
_OBJS = $(patsubst %,$O/%.o,$(SRC))
_DEPS = $(patsubst %,$O/%.o.d,$(SRC))

all: $(_TARGET)

$(_TARGET): $(_OBJS)
	@echo [LD] $(_TARGET)
	@$(CXX) $(LIBS) $(_OBJS) -o $(_TARGET)

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

