SHELL = /bin/sh
AR=ar rcs

.SUFFIXES: 
.SUFFIXES: .f .o


CXXFLAGS = -DDO_DEBUG -g0 -O3
FFLAGS =  -g0 -O3

LIBS  = `cernlib -safe packlib` -lgfortran
LDFLAGS = -static -L/usr/lib
CXXFLAGS += -DNO_PLOT

INCLUDE = -Wcpp -Irapidjson/include


#CXX = clang++-mp-3.5 -std=c++11
CXX = g++-4.8 -std=c++11 --fast-math -Wno-cpp -Df2cFortran
FF = gfortran-4.8 -Df2cFortran

headers := types.hpp util.hpp cernrun.hpp uparam.hpp pede_rms.hpp silicio.hpp algo.hpp cluster.hpp status.hpp \
	wrapper.hpp input_parser.hpp analisi.hpp my_histo.hpp

objects_fortran := init.o prepara_histo.o
objects_cxx :=  wrapper.o input_parser.o types.o analisi.o uparam.o my_histo.o

mains := detector.o pedestal.o

exe := $(patsubst %.o,%,$(mains))

all: $(exe)

$(objects_fortran): %.o : %.f $(headers)
	$(FF) $(FFLAGS) -c $< -I.

$(objects_cxx): %.o : %.cc $(headers)
	$(CXX) $(CXXLAGS) -c $< -I. $(INCLUDE)

$(mains): %.o : %.cc $(headers)
	$(CXX) $(CXXFLAGS) -c $< -I. $(INCLUDE)

$(exe): $(mains) $(objects_fortran)  $(objects_cxx) $(headers)
	$(CXX) $(LDFLAGS) -o $@ $@.o $(objects_fortran) $(objects_cxx) $(LIBS) 

.PHONY : clean

clean:
	\rm *.o $(exe)

