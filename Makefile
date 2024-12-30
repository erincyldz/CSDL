build/main: ./src/main.c
	mkdir -p build
	gcc -g -Wall -std=c99 -I./include ./src/main.c -o build/main -lSDL2main -lSDL2 -lm

clean:
	rm -rf build

.PHONY: clean