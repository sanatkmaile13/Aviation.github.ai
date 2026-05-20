##############################################################
#  AERO-QUEUE — Makefile
#  Usage:
#    make          → build the executable
#    make run      → build and run
#    make clean    → remove build artefacts
#    make valgrind → run with Valgrind memory check
##############################################################

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -Wshadow -O2
TARGET  = aero_queue
SRC     = src/aero_queue.c

.PHONY: all run clean valgrind

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo ""
	@echo "  Build successful → ./$(TARGET)"
	@echo ""

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
	@echo "  Cleaned build artefacts."

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all \
	         --track-origins=yes --verbose \
	         ./$(TARGET)
