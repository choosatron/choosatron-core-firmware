CXX ?= g++
CXXFLAGS ?= -g -Wall -W -Winline -ansi
CXXFLAGS += -Iinc -Isrc -Itests/UnitTest++/src
CXXFLAGS += -Ilib/elapsed_time/inc
CXXFLAGS += -Ilib/interval_timer/inc
CXXFLAGS += -Ilib/memory_free/inc
CXXFLAGS += -Ilib/sd/inc
CXXFLAGS += -Ilib/thermal_printer/inc

# annoying external dependency management
CXXFLAGS += -I../cdam-core-firmware/inc

LDFLAGS ?=
RM = rm

.SUFFIXES: .o .cpp

name        = Choosatron
lib         = src/lib$(name).a
test        = tests/test$(name)
testlibdir  = tests/UnitTest++
testlib     = $(testlibdir)/libUnitTest++.a
testrunner  = tests/Main.cpp

objects = src/Choosatron.o

testobjects = tests/TestChoosatron.o \
			  tests/TestParser.o


all: $(lib)

$(lib): $(objects)
	@echo Creating Choosatron library...
	@$(AR) cr $(lib) $(objects)

.cpp.o:
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	-@$(RM) $(objects) $(lib) $(testobjects) 2> /dev/null


############### tests #############

test: $(lib) $(testlib) $(testobjects)
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(test) $(testlib) \
    $(lib) $(testobjects) $(testrunner)
	@echo running unit tests...
	@./$(test)

$(testlib):
	$(MAKE) -C $(testlibdir)

testclean:
	$(MAKE) -C $(testlibdir) clean
