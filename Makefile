all: directories bin/main

directories:
	mkdir -p bin; mkdir -p build

bin/main: build/main.o build/arguments.o build/misc.o $(shell pwd)/modules/lib/replicators.so
	g++ -O3 $^ -o $@

build/main.o: ./src/main.cpp
	g++ -O3 -I./include -I./modules/include -c $< -o $@

build/arguments.o: ./src/arguments.cpp ./include/arguments.h
	g++ -O3 -I./include/ -fpic -c $< -o $@

build/misc.o: ./src/misc.cpp ./include/misc.h
	g++ -O3 -I./include/ -fpic -c $< -o $@

$(shell pwd)/modules/lib/replicators.so:
	cd modules; make

clean:
	rm -f *.o *.so main *~ \#* ./bin/* ./build/*; cd modules; make clean
