sns: sns.o pushUpdates.o readPosts.o userSimulation.o
	g++ -o sns sns.o pushUpdates.o readPosts.o userSimulation.o -lpthread

sns.o: sns.cpp sns.hpp
	g++ -c sns.cpp

pushUpdates.o: pushUpdates.cpp pushUpdates.hpp
	g++ -c pushUpdates.cpp

readPosts.o: readPosts.cpp readPosts.hpp
	g++ -c readPosts.cpp

userSimulation.o: userSimulation.cpp userSimulation.hpp
	g++ -c userSimulation.cpp

clean:
	rm -f *.o
