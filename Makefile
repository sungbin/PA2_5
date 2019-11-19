all: DNF

DNF: DNF_converter.c 
	gcc -o DNF DNF_converter.c 

clean:
	rm -rf DNF
