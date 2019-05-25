# Target library
lib := libfs.a

# Target files of library
parts := \
	disk.o \
	fs.o
src := $(patsubst %.x,%.o,$(parts))

all: $(lib) $(parts)

$(parts): $(src)
	gcc -c -Wall -Werror $(src)

$(lib): $(parts)
	ar rcs $(lib) $(parts)

clean:
	rm *.o *.a
