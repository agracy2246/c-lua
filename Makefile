build:
	gcc -std=c99 -Wall ./lib/lua/src/*.c ./src/*.c -o main

clean:
	rm ./main

run:
	./main

go: clean build run