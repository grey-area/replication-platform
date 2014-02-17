LDFLAGS += -L/home/andrew/local/boost_1_55_0/installed/lib

all: directories bin/main

directories:
	mkdir -p bin; mkdir -p build

bin/main: build/main.o build/config.o build/misc.o $(shell pwd)/modules/lib/replicators.so
	g++ -O3 $(LDFLAGS) $^ -o $@ -lboost_program_options

build/main.o: ./src/main.cpp
	g++ -O3 -I./include -I./modules/include -c $< -o $@

build/config.o: ./src/config.cpp ./include/config.h
	g++ -O3 -I/home/andrew/local/boost_1_55_0/installed/include -std=c++11 -I./include/  -fpic -c $< -o $@

build/misc.o: ./src/misc.cpp ./include/misc.h
	g++ -O3 -I./include/ -fpic -c $< -o $@

$(shell pwd)/modules/lib/replicators.so:
	cd modules; make

clean:
	rm -f *.o *.so main *~ \#* ./bin/* ./build/*; cd modules; make clean
