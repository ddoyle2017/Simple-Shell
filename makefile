simpleShell: simpleShell.o signalHandler.o
	g++ simpleShell.o signalHandler.o -o simpleShell

simpleShell.o: simpleShell.cpp signalHandler.h
	g++ -c simpleShell.cpp

signalHandler.o: signalHandler.cpp signalHandler.h
	g++ -w -c signalHandler.cpp

clean:
	rm -f *.o simpleShell
