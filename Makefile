all:
	gcc -fopenmp -std=c17 -Wall colorize.c -o colorize -lm

seq:
	gcc -fopenmp -std=c17 -Wall colorize-seq.c -o colorize -lm

clean:
	rm colorize