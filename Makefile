CC=gcc
CFLAGS=-Wall -O2 -Iinclude/
LIBS=-l gc
OBJS=bin/main.o bin/gc.o bin/object.o bin/read.o bin/print.o bin/builtin.o \
	 bin/superfasthash.o bin/hashtable.o bin/eval.o


plisp: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

bin/%.o: src/%.c | bin
	$(CC) $(CFLAGS) -c $^ -o $@

bin:
	dirname $(OBJS) | sort -u | xargs mkdir -p

.PHONY: debug clean

debug: CFLAGS=-Wall -g -Iinclude
debug: clean plisp

clean:
	-rm -r bin
	-rm  plisp
