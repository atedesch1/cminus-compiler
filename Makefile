build:
	flex cminus.l
	gcc -c *.c
	gcc -o cminus *.o -lfl

clean:
	$(RM) cminus *.o *~ *.yy.c