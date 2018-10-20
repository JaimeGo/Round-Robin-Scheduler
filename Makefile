all: scheduler

scheduler: main.o
	 gcc -o scheduler main.o

main.o: main.c
	 gcc -c main.c
     
clean:
	 rm main.o scheduler