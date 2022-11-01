build:
	bison -d cminus.y
	flex cminus.l
	gcc -c *.c
	gcc -o cminus *.o -ly -lfl

clean:
	$(RM) cminus *.o *~ *.yy.c *.tab.c *.tab.h
