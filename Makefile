all: muzak

muzak: muzak.cpp
	g++ -o muzak muzak.cpp -lSDL2 -pthread

clean:
	rm -f *.o *~ muzak

