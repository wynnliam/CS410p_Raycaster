CMP= ~/emsdk/emscripten/1.38.0/emcc
FLG= -s USE_SDL=2 -O3 -o src/raycaster.js
SRC= src/main.c

.PHONY: clean

all: $(SRC)
	$(CMP) $(SRC) $(FLG)

clean:
	rm src/*.js src/*.js.mem
