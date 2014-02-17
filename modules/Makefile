all: directories lib/replicators.so

directories:
	mkdir -p build; mkdir -p lib

lib/replicators.so: build/modelHandler.o build/Replicator1.o build/Grammar1.o build/BaseReplicator.o build/TestEnvironment1.o build/BaseEnvironment.o
	g++ -O3 -shared $^ -o $@


build/modelHandler.o: ./src/modelHandler.cpp ./include/modelHandler.h ./replicators/Replicator1/Replicator1.h ./replicators/Grammar1/Grammar1.h ./environments/TestEnvironment1/TestEnvironment1.h ../include/config.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ -I./replicators/Replicator1/ -I./replicators/Grammar1 -I./environments/TestEnvironment1 -fpic -c $< -o $@


build/Replicator1.o: ./replicators/Replicator1/Replicator1.cpp ./replicators/Replicator1/Replicator1.h ./include/BaseReplicator.h
	g++ -O3 -I./include/ -I../include/ -fpic -c $< -o $@

build/Grammar1.o: ./replicators/Grammar1/Grammar1.cpp ./replicators/Grammar1/Grammar1.h ./include/BaseReplicator.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ -fpic -c $< -o $@

build/BaseReplicator.o: ./src/BaseReplicator.cpp ./include/BaseReplicator.h ../include/config.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ -fpic -c $< -o $@


build/TestEnvironment1.o: ./environments/TestEnvironment1/TestEnvironment1.cpp ./environments/TestEnvironment1/TestEnvironment1.h ./include/BaseEnvironment.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ -fpic -c $< -o $@

build/BaseEnvironment.o: ./src/BaseEnvironment.cpp ./include/BaseEnvironment.h ../include/config.h
	g++ -O3 -std=c++11 -I./include/ -I../include/ -fpic -c $< -o $@

clean:
	rm -f *.o *.so *~ \#* ./build/*.o ./lib/*.so
