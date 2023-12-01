SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)

CFLAGS=-pedantic -Werror

%.o : %.c
	gcc $(CFLAGS) -c $< -o $@

aoc: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

.PHONY: run
run: aoc
	./aoc

.PHONY: clean
clean:
	rm ./src/*.o
	rm ./aoc

