CFLAGS=-g -pipe -O3 -Wall -Werror -pedantic --std=c1x

default: test demo_app

test: simple_options_test
	./simple_options_test

demo_app: demo_app.o simple_options.o
	$(CC) -o $@ simple_options.o demo_app.o

simple_options_test: simple_options.o simple_options_test.o
	$(CC) -o $@ simple_options.o simple_options_test.o

%.o: %.c simple_options.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o simple_options_test demo_app test-results.xml