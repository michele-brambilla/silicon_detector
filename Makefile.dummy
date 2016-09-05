SHELL = /bin/sh
AR=ar rcs

.SUFFIXES: 
.SUFFIXES: .f .o

CXX = clang++ -std=c++11
FF = gfortran-mp-4.9

CXXFLAGS = -O0 -m64 -I/sw/include -I/sw/include/cfortran -ggdb -DDO_DEBUG -Wcpp
FFLAGS = -O0 -m64 -ggdb

LIBS +=
LDFLAGS = 

INC = -Irapidjson/include



headers := types.hpp util.hpp cernrun.hpp uparam.hpp pede_rms.hpp silicio.hpp algo.hpp cluster.hpp

objects_fortran := init.o prepara_histo.o
objects_cxx := pedestal.o

mains := cernrun.o

exe := $(patsubst %.o,%,$(mains))

all: $(exe)

dummy : dummy.cc
	$(CXX) $(CXXLAGS) $(LIBS) $(LDFLAGS) -o dummy dummy.cc $(INC)

$(objects_fortran): %.o : %.f $(headers)
	$(FF) $(FFLAGS) -c $< -I.

$(objects_cxx): %.o : %.cc $(headers)
	$(CXX) $(CXXLAGS) -c $< -I.

$(mains): %.o : %.cc $(headers)
	$(CXX) $(CXXFLAGS) -c $< -I.

$(exe): $(mains) $(objects_fortran)  $(objects_cxx) $(headers)
	$(CXX) $(LIBS) $(LDFLAGS) -o $@ $@.o $(objects_fortran) $(objects_cxx)

.PHONY : clean

clean:
	\rm *.o cernrun

