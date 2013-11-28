all: lib/replicators.so

lib/replicators.so: build/instantiate_replicator.o build/Replicator1.o build/Replicator2.o build/BaseReplicator.o
	mkdir build; mkdir lib; g++ -shared $^ -o $@

build/instantiate_replicator.o: ./src/instantiate_replicator.cpp ./include/instantiate_replicator.h ./replicators/Replicator1/Replicator1.h ./replicators/Replicator1/Replicator1.h
	g++ -I./include/ -I./replicators/Replicator1/ -I./replicators/Replicator2/ -fpic -c $< -o $@

build/Replicator1.o: ./replicators/Replicator1/Replicator1.cpp ./replicators/Replicator1/Replicator1.h ./include/BaseReplicator.h
	g++ -I./include/ -fpic -c $< -o $@

build/Replicator2.o: ./replicators/Replicator2/Replicator2.cpp ./replicators/Replicator2/Replicator2.h ./include/BaseReplicator.h
	g++ -I./include/ -fpic -c $< -o $@

build/BaseReplicator.o: ./src/BaseReplicator.cpp ./include/BaseReplicator.h
	g++ -I./include/ -fpic -c $< -o $@

clean:
	rm -f *.o *.so *~ \#* ./build/*.o ./lib/*.so
