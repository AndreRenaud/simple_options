default: test demo_app

test: option_parser_test
	./option_parser_test

demo_app: demo_app.o option_parser.o
	$(CC) -o $@ option_parser.o demo_app.o

option_parser_test: option_parser.o option_parser_test.o
	$(CC) -o $@ option_parser.o option_parser_test.o

%.o: %.c option_parser.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o option_parser_test demo_app