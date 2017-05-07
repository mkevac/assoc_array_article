CXX=g++
CFLAGS=-std=c++11 -O3 -ffast-math -static-libstdc++ -static-libgcc
LDFLAGS=

all: hash_judy_test hash_judy_test_jemalloc

hash_judy_test: hash_judy_test.cpp thirdparty/opt/judy/lib/libJudy.a thirdparty/opt/sparsehash/include/sparsehash/sparse_hash_map thirdparty/src/t1ha/libt1ha.a
	$(CXX) $(CFLAGS) \
		-I . \
		-I thirdparty/opt/judy/include \
		-I thirdparty/src/sparsepp \
		-I thirdparty/src/xxHash \
		-I thirdparty/src/t1ha \
		-I thirdparty/opt/sparsehash/include \
		-o hash_judy_test \
		hash_judy_test.cpp \
		thirdparty/opt/judy/lib/libJudy.a \
		thirdparty/src/t1ha/libt1ha.a \
		thirdparty/src/xxHash/xxhash.c

hash_judy_test_jemalloc: hash_judy_test.cpp thirdparty/opt/judy/lib/libJudy.a thirdparty/opt/sparsehash/include/sparsehash/sparse_hash_map thirdparty/opt/jemalloc/lib/libjemalloc.a thirdparty/src/t1ha/libt1ha.a
	$(CXX) $(CFLAGS) \
		-I . \
		-I thirdparty/opt/judy/include \
		-I thirdparty/src/sparsepp \
		-I thirdparty/src/xxHash \
		-I thirdparty/src/t1ha \
		-I thirdparty/opt/sparsehash/include \
		-o hash_judy_test_jemalloc \
		-pthread \
		hash_judy_test.cpp \
		thirdparty/opt/judy/lib/libJudy.a \
		thirdparty/src/t1ha/libt1ha.a \
		thirdparty/opt/jemalloc/lib/libjemalloc.a \
		thirdparty/src/xxHash/xxhash.c

thirdparty/src/t1ha/libt1ha.a:
	cd thirdparty/src/t1ha && make

thirdparty/opt/judy/lib/libJudy.a: thirdparty/src/judy-1.0.5
	cd thirdparty/src/judy-1.0.5 &&	./configure --prefix=${CURDIR}/thirdparty/opt/judy && make && make install

thirdparty/src/judy-1.0.5: thirdparty/src/Judy-1.0.5.tar.gz
	cd thirdparty/src && tar xf Judy-1.0.5.tar.gz

thirdparty/opt/jemalloc/lib/libjemalloc.a: thirdparty/src/jemalloc-4.5.0
	cd thirdparty/src/jemalloc-4.5.0 && ./configure --prefix=${CURDIR}/thirdparty/opt/jemalloc && make -j8 && make install

thirdparty/src/jemalloc-4.5.0: thirdparty/src/jemalloc-4.5.0.tar.bz2
	cd thirdparty/src && tar xf jemalloc-4.5.0.tar.bz2

thirdparty/opt/sparsehash/include/sparsehash/sparse_hash_map:
	cd thirdparty/src/sparsehash &&	./configure --prefix=${CURDIR}/thirdparty/opt/sparsehash && make -j8 && make install

clean:
	rm hash_judy_test
	rm hash_judy_test_jemalloc
	rm -rf thirdparty/opt/judy
	rm -rf thirdparty/opt/sparsehash
	rm -rf thirdparty/opt/jemalloc
	rm -rf thirdparty/src/jemalloc-4.5.0
	rm -rf thirdparty/src/judy-1.0.5
