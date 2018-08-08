cross: cross.o 
	gcc -g -Wall *.o -o cross -lm

cross.o: cross.c
	gcc -g -c -std=c99 -Wall cross.c 

.PHONY: clean

clean:
	rm *.o cross
