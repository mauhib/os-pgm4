.PHONY: clean run test
CFLAGS=-Wall -g
TARGET=pgm4
ARGS =log

$(TARGET): $(TARGET).c $(TARGET).h

clean:
	rm -f *.o log

run:
	./$(TARGET) $(ARGS)

test:
	make
	valgrind ./$(TARGET) $(ARGS)
