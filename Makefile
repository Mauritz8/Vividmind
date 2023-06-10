CC=clang
CFLAGS=-I include/
DEPS = $(wildcard include/*.h)
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))
EXECUTABLE = main

all: $(EXECUTABLE)

obj/%.o: src/%.c $(DEPS)
	mkdir -p obj
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECUTABLE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXECUTABLE) obj/*.o
