LDFLAGS += -L/home/andrew/local/boost_1_55_0/installed/lib

all: directories replication

directories:
	mkdir -p build

replication: build/replication.o build/config.o build/misc.o $(shell pwd)/modules/lib/replicators.so
	g++ -O3 $(LDFLAGS) $^ -o $@ -lboost_program_options; rm build -rf

build/replication.o: ./src/replication.cpp
	g++ -O3 -I/home/andrew/local/boost_1_55_0/installed/include -I./include -I./modules/include -c $< -o $@

build/config.o: ./src/config.cpp ./include/config.h
	g++ -O3 -I/home/andrew/local/boost_1_55_0/installed/include -std=c++11 -I./include/  -fpic -c $< -o $@

build/misc.o: ./src/misc.cpp ./include/misc.h
	g++ -O3 -I./include/ -fpic -c $< -o $@

$(shell pwd)/modules/lib/replicators.so:
	cd modules; make

clean:
	rm -f *.o *.so replication *~ \#* ./build/*; cd modules; make clean
