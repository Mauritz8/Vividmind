CC=clang
CFLAGS=-I include/
DEPS = $(wildcard include/*.h)
SRC = $(wildcard src/*.c)
SRC := $(filter-out src/main.c, $(SRC))
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))
TEST_SRC = $(wildcard src/test/*.c)
TEST_OBJ = $(patsubst src/test/*.c,obj/%.o,$(TEST_SRC))
EXECUTABLE = main
TEST_EXECUTABLE = tests
LIBS = -lcunit

all: $(EXECUTABLE)

obj/%.o: src/%.c $(DEPS)
	mkdir -p obj
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECUTABLE): obj/main.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

obj/%.o: src/test/%.c $(DEPS)
	mkdir -o obj
	$(CC) -c -o $@ $< $(CFLAGS)

$(TEST_EXECUTABLE): $(OBJ) $(TEST_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE) obj/*.o

run_tests: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)


.PHONY: run clean run_tests
