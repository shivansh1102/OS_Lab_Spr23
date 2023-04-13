CXXFLAGS=-Wall

all: library demo

library: goodmalloc.o
	ar rcs -o libgoodmalloc.a goodmalloc.o

goodmalloc.o: goodmalloc.cpp goodmalloc.hpp

install:
	cp -p goodmalloc.hpp /usr/local/include
	cp -p libgoodmalloc.a /usr/local/lib

clean:
	rm -f goodmalloc.o libgoodmalloc.a demo mergesort

demo: demo1.cpp
	g++ $(CXXFLAGS) demo1.cpp -o demo -L. -lgoodmalloc

mergesort: mergesort2.cpp
	g++ $(CXXFLAGS) mergesort2.cpp -o mergesort -L. -lgoodmalloc
