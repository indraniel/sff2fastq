.PHONY: clean

sfff2fastq: main.o sff.o
	gcc -g -o sff2fastq main.o sff.o

main.o: main.c sff.h
	gcc -g -I. -c main.c

sff.o: sff.c sff.h
	gcc -D__GENOME__ -g -I. -c sff.c

clean:
	rm sff2fastq main.o sff.o
