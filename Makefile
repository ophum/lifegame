
a.out: main.c
	gcc main.c -lpthread -lcurses

.PHONY: run
run: a.out
	./a.out

.PHONY: clean
clean: a.out
	rm a.out
