CMP= ~/emsdk/emscripten/1.38.0/emcc
FLG= -s WASM=1 -s USE_SDL=2 -O3 --preload-file ./src/assests -o ./public/raycaster.js

SRC= ./src/*.c

.PHONY: clean

all: $(SRC)
	$(CMP) $(SRC) $(FLG)

all_out: $(SRC)
	gcc -g -Wall $(SRC) -lm -lSDL2 

clean:
	rm public/raycaster.*
