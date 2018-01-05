cross: cross.o 
	gcc -Wall *.o -o cross -lm

cross.o: cross.c cross.h
	gcc -g -c -Wall cross.c 

.PHONY: clean

clean:
	rm *.o cross
