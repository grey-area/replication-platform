$(CXX) = g++ -O3

all: directories lib/replicators.so

directories:
	mkdir -p build; mkdir -p lib

lib/replicators.so: build/modelHandler.o build/Replicator1.o build/Grammar1.o build/BaseReplicator.o
	$(CXX) -shared $^ -o $@

build/modelHandler.o: ./src/modelHandler.cpp ./include/modelHandler.h ./replicators/Replicator1/Replicator1.h ./replicators/Grammar1/Grammar1.h
	$(CXX) -std=c++11 -I./include/ -I./replicators/Replicator1/ -I./replicators/Grammar1 -fpic -c $< -o $@

build/Replicator1.o: ./replicators/Replicator1/Replicator1.cpp ./replicators/Replicator1/Replicator1.h ./include/BaseReplicator.h
	$(CXX) -I./include/ -fpic -c $< -o $@

build/Grammar1.o: ./replicators/Grammar1/Grammar1.cpp ./replicators/Grammar1/Grammar1.h ./include/BaseReplicator.h
	$(CXX) -std=c++11 -I./include/ -fpic -c $< -o $@

build/BaseReplicator.o: ./src/BaseReplicator.cpp ./include/BaseReplicator.h
	$(CXX) -std=c++11 -I./include/ -fpic -c $< -o $@

clean:
	rm -f *.o *.so *~ \#* ./build/*.o ./lib/*.so
