CXX ?= g++
CXXFLAGS ?= -g -Wall -W -Winline -ansi
CXXFLAGS += -Isrc -Itests/UnitTest++/src

# annoying external dependency management
CXXFLAGS += -I../core-firmware/inc

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

testobjects = tests/TestChoosatron.o


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
