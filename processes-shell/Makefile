
wish: main.o common.o
	cc -Wall -g -o wish main.o common.o

test: common.o
	cc -Wall -g -o test common.o

main.o: main.c
	cc -Wall -g -c -o main.o main.c
common.o: common.c
	cc -Wall -g -c -o common.o common.c

clean:
	rm *.o
	rm wish
	rm test
	rm -rf *.dSYM