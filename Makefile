edit : main.o 
	cc -o main main.o 


main.o : main.c
	cc -c main.c 

clean : 
	rm edit main.o
