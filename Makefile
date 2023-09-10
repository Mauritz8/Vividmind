CC=clang++
CFLAGS=-std=c++17 -I include/ -I include/engine/ -I tests/include -g
DEPS = $(wildcard include/*.h include/engine/*.h)
SRC = $(wildcard src/*.cpp src/engine/*.cpp)
SRC := $(filter-out src/main.cpp src/terminal_game.cpp, $(SRC))
OBJ = $(patsubst src/%.cpp,obj/%.o,$(SRC))
TEST_SRC = $(wildcard tests/src/*.cpp)
TEST_OBJ = $(patsubst tests/src/*.cpp,tests/obj/%.o,$(TEST_SRC))
ENGINE = engine
TERMINAL_GAME = terminal_game
TEST_EXECUTABLE = unit_tests
LIBS = -lcunit

all: $(ENGINE) $(TERMINAL_GAME)

obj/%.o: src/%.cpp $(DEPS)
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ENGINE): obj/main.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(TERMINAL_GAME): obj/terminal_game.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

obj/%.o: src/test/%.cpp $(DEPS)
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TEST_EXECUTABLE): $(OBJ) $(TEST_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: $(ENGINE)
	./$(ENGINE)

clean:
	rm -f $(ENGINE) $(TERMINAL_GAME) $(TEST_EXECUTABLE) obj/*.o

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)


.PHONY: run clean run_tests
