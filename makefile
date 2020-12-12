main: main.o game.o ultimate.o
	gcc main.o game.o ultimate.o -o main
main.o: main.c
	gcc -c main.c
game.o: game.c game.h
	gcc -c game.c
ultimate.o: ultimate.c ultimate.h
	gcc -c ultimate.c
clean:
	rm *.o main