CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -Iinclude
LDFLAGS =

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN = cfg2yacc

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean tests run report

all: $(BIN)

$(BIN): $(BUILD_DIR) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(wildcard $(INC_DIR)/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN) out.* *.tab.* *.output parser logs tests/results.txt

tests: $(BIN)
	./tests/run_tests.sh

run: $(BIN)
	./$(BIN) -i samples/expr.ebnf -o out.y --emit-lex out.l

report:
	@if command -v pandoc >/dev/null 2>&1; then \
		pandoc -s -o report.pdf REPORT.md; \
		echo "report.pdf generated"; \
	else \
		echo "pandoc not found; please convert REPORT.md manually"; \
	fi


