# Target library
lib := libfs.a

# Target files of library
parts := \
	disk.o \
	fs.o

all: $(lib) $(parts)

disk.o: disk.c
	gcc -c -Wall -Werror disk.c

fs.o: fs.c
	gcc -c -Wall -Werror fs.c

libfs.a: disk.o fs.o
	ar rcs $(lib) disk.o fs.o

clean:
	rm *.o *.a
