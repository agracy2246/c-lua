build:
	gcc -std=c99 -Wall ./lib/lua/src/*.c ./src/main.c -o main -lm

clean:
	rm ./main

run:
	./main

go: build run clean

CFLAGS = -std=c99 -Wall -I ./lib/lua/src
LUA_SRCS = ${wildcard ./lib/lua/src/*.c}
SRC_SRCS = ${wildcard ./src/*.c}
OUT = main

all:
	$(CC) $(CFLAGS) $(LUA_SRCS) $(SRC_SRCS) -o $(OUT)
