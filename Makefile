all: bin/main

bin/main: build/main.o $(shell pwd)/modules/lib/replicators.so
	g++ $^ -o $@

build/main.o: ./src/main.cpp
	g++ -I./include -I./modules/include -c $< -o $@

$(shell pwd)/modules/lib/replicators.so:
	cd modules;make

clean:
	rm -f *.o *.so main *~ \#* ./bin/* ./build/*; cd modules; make clean
