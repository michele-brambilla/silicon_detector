SHELL = /bin/sh
AR=ar rcs

.SUFFIXES: 
.SUFFIXES: .f .o

LIBS += -lpawlib -lpacklib -lgfortran

CXXFLAGS = -O0 -m64 -I/sw/include -I/sw/include/cfortran -ggdb -DDO_DEBUG -Wcpp
FFLAGS = -O0 -m64 -ggdb

LDFLAGS = -L/opt/local/lib/gcc49/ -L/sw/lib/ -L/usr/lib

#CXX = clang++-mp-3.5 -std=c++11
CXX = g++-mp-4.9 -std=c++11
FF = gfortran-mp-4.9

headers := types.hpp util.hpp cernrun.hpp uparam.hpp pede_rms.hpp silicio.hpp algo.hpp cluster.hpp

objects_fortran := init.o prepara_histo.o
objects_cxx := pedestal.o

mains := cernrun.o

exe := $(patsubst %.o,%,$(mains))

all: $(exe)

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

