build:
	gcc -std=c99 -Wall -I ./lib/sdl2/include ./lib/lua/src/*.c ./src/*.c -o main

clean:
	rm ./main

run:
	./main

go: build run clean