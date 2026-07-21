CPP_SOURCES = $(shell find ./client -name "*.cpp")
OBJ_FILES = ${CPP_SOURCES:.cpp=.o}

build: prepare $(OBJ_FILES) server
	g++ out/obj/* imgui_comp.o -o ./out/client.bin -lSDL2 -lSDL2_net -lglfw
	gcc ./server/main.c -o ./out/server.bin -lSDL2 -lSDL2_net

prepare:
	mkdir -p out/obj

run_server:
	./out/server.bin
run_client:
	./out/client.bin

%.o: %.cpp
	g++ -I./client/include -c $< -o out/obj/$(notdir $@)

