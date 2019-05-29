# Target library
lib := libfs.a

# Target files of library
parts := \
	disk.o \
	fs.o

CFLAGS := -c -Wall -Werror

src := $(patsubst %.o,%.c,$(parts))

all: $(lib) $(parts)

$(parts): $(src)
	gcc $(CFLAGS) $(src)

$(lib): $(parts)
	ar rcs $(lib) $(parts)

clean:
	rm *.o *.a
