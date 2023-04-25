main: main.o guest.o cleaningStaff.o
	g++ guest.o cleaningStaff.o main.o -lpthread -o main

main.o: main.cpp main.hpp
	g++ -c main.cpp

guest.o: guest.cpp main.hpp
	g++ -c guest.cpp

cleaningStaff.o: cleaningStaff.cpp main.hpp
	g++ -c cleaningStaff.cpp

clean:
	rm -rf *.o
