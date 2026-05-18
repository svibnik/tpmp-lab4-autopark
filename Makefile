CC = gcc
CFLAGS = -Wall -Wextra -Isrc
LDFLAGS = -lsqlite3 -lcunit

SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/db.c $(SRC_DIR)/auth.c \
          $(SRC_DIR)/queries.c $(SRC_DIR)/operations.c $(SRC_DIR)/utils.c
          
TEST_SOURCES = $(TEST_DIR)/test_autopark.c $(SRC_DIR)/db.c $(SRC_DIR)/auth.c \
               $(SRC_DIR)/queries.c $(SRC_DIR)/operations.c $(SRC_DIR)/utils.c

TARGET = $(BIN_DIR)/autopark
TEST_TARGET = $(BIN_DIR)/test_autopark

all: $(TARGET)

$(TARGET): $(SOURCES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all test clean