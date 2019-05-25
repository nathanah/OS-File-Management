# Target library
lib := libfs.a

# Target files of library
parts := \
	disk \
	fs

all: $(lib) $(parts).o

$(parts).o: $(parts).c
	gcc -c -Wall -Werror $(parts).c

$(lib): $(parts).o
	ar rcs $(lib) $(parts).o

clean:
	rm *.o *.a
