cross: cross.o 
	gcc -Wall *.o -o cross -lm
	./cross

cross.o: cross.c cross.h
	gcc -c -Wall cross.c 

.PHONY: clean

clean:
	rm *.o cross
