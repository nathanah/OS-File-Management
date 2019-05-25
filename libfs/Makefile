# Target library
lib := libfs.a

# Target files of library
parts := \
	disk.o \
	fs.o

all: $(lib) $(parts)

$(parts).o: $(parts).c
	gcc -c -Wall -Werr $(parts).c

$(lib): $(parts).o
	ar rcs $(lib) $(parts).o

clean:
	rm *.o *.a
