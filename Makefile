EXE	:= main
EXEDIR := build
SRCDIR	:= src
INCLUDEDIR	:= src
OBJDIR	:= build

# define dependency directory
DEPDIR	:= build

# define lib directory

LIBFILES := systemc

# output binary
BIN := $(EXEDIR)/$(EXE)

# includes directories
INCLUDEDIRS	:= $(shell find $(INCLUDEDIR) -type d)
INCLUDES := $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# source files
SOURCEDIRS	:= $(shell find $(SRCDIR) -type d)
SRCS := $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))

# C/C++ libs
LIBS := $(patsubst %,-l%, $(LIBFILES:%/=%))

# object files, auto generated from source files
OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))
# dependency files, auto generated from source files
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

# compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)

CC := g++
CXX := g++
LD := g++

CFLAGS := -std=c17
CXXFLAGS :=
CPPFLAGS := -g -Wall -Wextra -pedantic -std=c++17
LDFLAGS :=
LDLIBS := $(LIBS)

ifeq ($(shell uname), Darwin)
    INCLUDES += -I/opt/homebrew/include
    LDFLAGS += -L/opt/homebrew/lib
endif

# flags required for dependency generation; passed to compilers
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td

# compile C source files
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c -o $@
# compile C++ source files
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDES) $(CPPFLAGS) -c -o $@
# link object files to binary
LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) -o $@
# precompile step
PRECOMPILE =
# postcompile step
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

########## RULES ##########################################

# Build only
all: $(BIN)

# Build and Run program
run: all
	./$(BIN)

# Build, run program and launch GTKWave
gtkwave: run
	gtkwave -n -o

# Clean project
.PHONY: clean
clean:
	$(RM) -r $(OBJDIR) $(DEPDIR)

.PHONY: help
help:
	@echo -e 'available targets:\n - all (Build only),\n - run (Build and run program),\n - gtkwave (Build, run program and launch GTKWave),\n - clean\n'

$(BIN): $(OBJS)
	$(LINK.o) $^

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cc
$(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cxx
$(OBJDIR)/%.o: %.cxx $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cc) $<
	$(POSTCOMPILE)

.PRECIOUS: $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)