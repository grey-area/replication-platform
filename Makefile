all: directories replication

profile: directories replication_profile

directories:
	mkdir -p build


replication: build/replication.o build/config.o build/misc.o $(shell pwd)/modules/lib/modules.so
	g++ -O3 -o $@ $^ -lsqlite3; rm build -rf 

build/replication.o: ./src/replication.cpp
	g++ -O3 -I./include -I./modules/include -c $< -o $@ 

build/config.o: ./src/config.cpp ./include/config.h
	g++ -O3 -I./include/  -fpic -c $< -o $@

build/misc.o: ./src/misc.cpp ./include/misc.h
	g++ -O3 -I./include/ -fpic -c $< -o $@

$(shell pwd)/modules/lib/modules.so:
	cd modules; make


replication_profile: build/replication_profile.o build/config_profile.o build/misc_profile.o $(shell pwd)/modules/lib/modules_profile.so
	g++ -pg -o $@ $^ -lsqlite3; rm build -rf 

build/replication_profile.o: ./src/replication.cpp
	g++ -pg -I./include -I./modules/include -c $< -o $@ 

build/config_profile.o: ./src/config.cpp ./include/config.h
	g++ -pg -I./include/  -fpic -c $< -o $@

build/misc_profile.o: ./src/misc.cpp ./include/misc.h
	g++ -pg -I./include/ -fpic -c $< -o $@

$(shell pwd)/modules/lib/modules_profile.so:
	cd modules; make profile


clean:
	rm -f *.o *.so replication replication_profile *~ \#* ./build/*; cd modules; make clean
