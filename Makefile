SHELL = /bin/sh
AR=ar rcs

.SUFFIXES: 
.SUFFIXES: .f .o


CXXFLAGS = -O3 -DDO_DEBUG #-ggdb -O0
FFLAGS = -O3 #-ggdb -O0

LIBS += `cernlib -safe pawlib packlib graflib` -lgfortran
LDFLAGS =  -L/usr/lib

#LIBS += `cernlib -safe pawlib packlib` -lgfortran
#LDFLAGS = -static -L/usr/lib

INCLUDE = -Wcpp -Irapidjson/include


#CXX = clang++-mp-3.5 -std=c++11
CXX = g++-4.8 -std=c++11 --fast-math -Wno-cpp -Df2cFortran
FF = gfortran-4.8 -Df2cFortran

headers := types.hpp util.hpp cernrun.hpp uparam.hpp pede_rms.hpp silicio.hpp algo.hpp cluster.hpp status.hpp

objects_fortran := init.o prepara_histo.o
objects_cxx := pedestal.o

mains := basculo.o

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
	\rm *.o basculo

