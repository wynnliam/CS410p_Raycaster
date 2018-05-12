CMP= ~/emsdk/emscripten/1.38.0/emcc
FLG= -s USE_SDL=2 -O3 -o public/raycaster.js
SRC= src/main.c

.PHONY: clean

all: $(SRC)
	$(CMP) $(SRC) $(FLG)

clean:
	rm public/*.js public/*.js.mem
