CC = gcc
CFLAGS = -c -Wall -Werror -Wextra -std=c11 -g

all: s21_cat

s21_cat: s21_cat.o
	$(CC) s21_cat.o -o s21_cat

s21_cat.o: s21_cat.c
	$(CC) $(CFLAGS) s21_cat.c -o s21_cat.o

func: ./tests/test_func_cat.sh
	sh ./tests/test_func_cat.sh
	
leaks: ./tests/test_leak_cat.sh
	sh ./tests/test_leak_cat.sh

clean:
	rm -rf *.o s21_cat
 
rebuild: clean all
