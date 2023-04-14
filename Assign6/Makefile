CXXFLAGS=-Wall

all: library mergesort

library: goodmalloc.o
	ar rcs -o libgoodmalloc.a goodmalloc.o

goodmalloc.o: goodmalloc.cpp goodmalloc.hpp

install:
	cp -p goodmalloc.hpp /usr/local/include
	cp -p libgoodmalloc.a /usr/local/lib

clean:
	rm -f goodmalloc.o libgoodmalloc.a demo mergesort


mergesort: mergesort2.cpp
	g++ $(CXXFLAGS) mergesort2.cpp -o mergesort -L. -lgoodmalloc
