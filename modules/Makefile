all: directories lib/replicators.so

directories:
	mkdir -p build; mkdir -p lib

lib/replicators.so: build/modelHandler.o build/Replicator1.o build/Grammar1.o build/BaseReplicator.o
	g++ -O3 -shared $^ -o $@ -pg

build/modelHandler.o: ./src/modelHandler.cpp ./include/modelHandler.h ./replicators/Replicator1/Replicator1.h ./replicators/Grammar1/Grammar1.h
	g++ -O3 -std=c++11 -I./include/ -I./replicators/Replicator1/ -I./replicators/Grammar1 -fpic -c $< -o $@ -pg

build/Replicator1.o: ./replicators/Replicator1/Replicator1.cpp ./replicators/Replicator1/Replicator1.h ./include/BaseReplicator.h
	g++ -O3 -I./include/ -fpic -c $< -o $@ -pg

build/Grammar1.o: ./replicators/Grammar1/Grammar1.cpp ./replicators/Grammar1/Grammar1.h ./include/BaseReplicator.h
	g++ -O3 -std=c++11 -I./include/ -fpic -c $< -o $@ -pg

build/BaseReplicator.o: ./src/BaseReplicator.cpp ./include/BaseReplicator.h
	g++ -O3 -std=c++11 -I./include/ -fpic -c $< -o $@ -pg

clean:
	rm -f *.o *.so *~ \#* ./build/*.o ./lib/*.so
