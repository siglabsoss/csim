CXXFLAGS += -std=c++11

INC=/usr/local/systemc-2.3.1/include/ ../
LIB=/usr/local/systemc-2.3.1/lib-linux64/ ../../build/filters/ ../../build/types ../../build/core
INC_PARAMS=$(foreach d, $(INC), -I$d)
LIB_PARAMS=$(foreach d, $(LIB), -L$d)
LIBS=systemc sigfilters sigtypes sigcore
LIBS_PARAMS=$(foreach d, $(LIBS), -l$d)

%.oct: %.cpp
	export CXXFLAGS=$(CXXFLAGS) && mkoctfile $(INC_PARAMS) $(LIB_PARAMS) $(LIBS_PARAMS) $? 

all: oct_fixedfir.oct

.PHONY: clean
clean:
	rm *.o*
