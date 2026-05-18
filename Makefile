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

COV_FLAGS = -fprofile-arcs -ftest-coverage
COV_DIR = coverage

all: $(TARGET)

$(TARGET): $(SOURCES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SOURCES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(LDFLAGS)

coverage: CFLAGS += $(COV_FLAGS)
coverage: LDFLAGS += $(COV_FLAGS) -lgcov
coverage: $(TEST_TARGET)
	mkdir -p $(COV_DIR)
	$(TEST_TARGET)
	lcov --capture --directory . --output-file $(COV_DIR)/coverage.info
	genhtml $(COV_DIR)/coverage.info --output-directory $(COV_DIR)/html
	@echo ""
	@echo "=== Coverage report ==="
	@cat $(COV_DIR)/coverage.info | grep -E "lines|functions" | head -2

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR) $(COV_DIR) *.gcda *.gcno *.gcov

.PHONY: all test coverage clean
