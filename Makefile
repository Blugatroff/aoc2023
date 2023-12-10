SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)

CFLAGS=-pedantic -Werror

%.o : %.c
	gcc -O3 $(CFLAGS) -c $< -o $@

aoc: $(OBJ)
	gcc -O3 $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm ./src/*.o
	rm ./aoc

