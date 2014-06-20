all: directories replication

directories:
	mkdir -p build

replication: build/replication.o build/config.o build/misc.o $(shell pwd)/modules/lib/modules.so
	g++ -O3 -Ofast -o $@ $^ -lsqlite3; rm build -rf 

build/replication.o: ./src/replication.cpp
	g++ -O3 -Ofast -I./include -I./modules/include -c $< -o $@ 

build/config.o: ./src/config.cpp ./include/config.h
	g++ -O3 -Ofast -I./include/  -fpic -c $< -o $@

build/misc.o: ./src/misc.cpp ./include/misc.h
	g++ -O3 -Ofast -I./include/ -fpic -c $< -o $@

$(shell pwd)/modules/lib/modules.so:
	cd modules; make

clean:
	rm -f *.o *.so replication *~ \#* ./build/*; cd modules; make clean
