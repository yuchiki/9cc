CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(CFLAGS)

test: 9cc
	./9cc -test
	./test.sh

clean:
	rm -f 9cc *.o tmp*
