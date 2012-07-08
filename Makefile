all: muzak

muzak: muzak.cpp
	g++ -o muzak muzak.cpp -lSDL -pthread

clean:
	rm -f *.o *~ muzak

