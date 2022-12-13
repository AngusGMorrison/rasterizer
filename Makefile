build:
	gcc -Wall -std=c17 -lSDL2 ./src/*.c -o rasterizer

run:
	./rasterizer

brun: build run

clean:
	rm rasterizer
