flags=$(shell gpgme-config --libs --cflags) -std=c99 -Wall -Werror -Wextra -pedantic -lgpg-error

gpgme-sigs: gpgme-sigs.c
	gcc -D_FILE_OFFSET_BITS=64 -g ${flags} $< -o $@

clean:
	rm -f gpgme-sigs
