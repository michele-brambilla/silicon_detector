SHELL = /bin/sh
AR=ar rcs

.SUFFIXES: 
.SUFFIXES: .f .o


CXXFLAGS = -O3  -DDO_DEBUG -Df2cFortran
FFLAGS = -O3 #-ggdb -O0

LIBS += -lpawlib -lpacklib -lgfortran
LDFLAGS = -L/opt/local/lib/gcc49/ -L/sw/lib/ -L/usr/lib

INCLUDE = -I/sw/include -I/sw/include/cfortran -Wcpp -Irapidjson/include


#CXX = clang++-mp-3.5 -std=c++11
CXX = g++ -std=c++11 --fast-math -Wno-cpp
FF = gfortran

headers := types.hpp util.hpp cernrun.hpp uparam.hpp pede_rms.hpp silicio.hpp algo.hpp cluster.hpp

objects_fortran := init.o prepara_histo.o
objects_cxx := pedestal.o

mains := cernrun_basculo.o cernrun_pedestal.o

exe := $(patsubst %.o,%,$(mains))

all: $(exe)

$(objects_fortran): %.o : %.f $(headers)
	$(FF) $(FFLAGS) -c $< -I.

$(objects_cxx): %.o : %.cc $(headers)
	$(CXX) $(CXXLAGS) -c $< -I. $(INCLUDE)

$(mains): %.o : %.cc $(headers)
	$(CXX) $(CXXFLAGS) -c $< -I. $(INCLUDE)

$(exe): $(mains) $(objects_fortran)  $(objects_cxx) $(headers)
	$(CXX)  -o $@ $@.o $(objects_fortran) $(objects_cxx) $(LIBS) $(LDFLAGS)

.PHONY : clean

clean:
	\rm *.o cernrun

