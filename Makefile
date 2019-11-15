all: CNF

CNF: CNF_convert.c
	gcc -o CNF CNF_convert.c

clean:
	rm -rf CNF
