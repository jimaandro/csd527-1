CC = gcc
CFLAGS = -m32 -g
SRC_FILES = thread.c chan.c sem.c swtch_x86.S

all: clean test

test: $(TEST_FILE)
	$(CC) $(CFLAGS) $(TEST_FILE) $(SRC_FILES) -o test

run: test
	./test

clean:
	rm -f test

test.c:
	@echo "Please provide a test file with 'make TEST_FILE=your_test.c'"