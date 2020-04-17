CC       = gcc
CFLAGS   = -std=c99 -Wall -D_POSIX_C_SOURCE=201112L -D_XOPEN_SOURCE=500 -D_GNU_SOURCE -static
LD       = gcc
LDFLAGS  = -static

LIBFLAGS = -Llibs -Ilibs

OBJS     = obj/arrays.o obj/linked.o obj/streams.o obj/strings.o
DEPS     = $(OBJS:%.o=%.d)

all: $(BIN_NAME) test

demo: obj/demo.o $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

test: $(OBJS) obj/do-test.o obj/linked-test.o obj/streams-test.o obj/strings-test.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBFLAGS) -lviscocity

obj/%-test.o: src/%-test.c obj
	$(CC) $(CFLAGS) $(LIBFLAGS) -MMD -c -o $@ $<
	

valgrind: CFLAGS += -static -g
valgrind: clean test
	valgrind --leak-check=yes ./test

-include $(DEPS)

obj/%.o: src/%.c obj
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

obj:
	@mkdir -p obj

clean:
	@echo "Cleaning up..."
	@rm -f obj/*.o
	@rm -f obj/*.d
	@rm -f test
	@rm -f demo
