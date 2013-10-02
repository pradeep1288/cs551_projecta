projecta: manager.o client.o
	gcc -o projecta -g -lm manager.o client.o

manager.o: manager.c client.h
	gcc -g -c -Wall manager.c

client.o: client.c client.h 
	gcc -g -c -Wall client.c

clean:
	rm -f *.o projecta *.out
