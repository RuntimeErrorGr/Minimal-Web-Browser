build:
	gcc -Wall -g FunctiiCoada.c FunctiiStiva.c FunctiiLista.c  utils.c main.c -o tema2
run:
	./tema2
clean:
	rm -rf tema2
