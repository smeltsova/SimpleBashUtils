CC = gcc
CFLAGS = -c -Wall -Werror -Wextra -std=c11 -D_GNU_SOURCE -g

all: s21_grep

s21_grep: s21_grep.o
	$(CC) s21_grep.o -o s21_grep

s21_grep.o: s21_grep.c
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep.o

func: ./tests/test_func_grep.sh
	sh ./tests/test_func_grep.sh
	
leaks: ./tests/test_leak_grep.sh
	sh ./tests/test_leak_grep.sh

clean:
	rm -rf *.o s21_grep
 
rebuild: clean all
