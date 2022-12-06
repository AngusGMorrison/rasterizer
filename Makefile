build:
	gcc -Wall -std=c17 -lSDL2 ./src/*.c -o rasterizer

run:
	./rasterizer

clean:
	rm rasterizer
