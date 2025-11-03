# Makefile for Python-based Compiler-Generator System
CC = gcc
CFLAGS = -Wall -g
PYTHON = python3
GENERATOR_SCRIPT = generator.py
VISUALIZER_SCRIPT = visualize_tree.py
DEF_FILE = samples/sample5_social_media/S5_analyzer.def

# Parse-tree library
LIB_SRCS = ast.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

# Generated files
LEXER_SOURCE = lexer.l
PARSER_SOURCE = parser.y
LEXER_OUTPUT = lex.yy.c
PARSER_OUTPUT = y.tab.c
PARSER_HEADER = y.tab.h

# Final compiler executable
TARGET = custom_compiler

# Default target
all: $(TARGET)

# Generate lexer.l and parser.y from .def file using Python generator
$(LEXER_SOURCE) $(PARSER_SOURCE): $(DEF_FILE) $(GENERATOR_SCRIPT)
	@echo "Generating lexer and parser from $(DEF_FILE)..."
	$(PYTHON) $(GENERATOR_SCRIPT) $(DEF_FILE)

# Generate C code from lexer specification
$(LEXER_OUTPUT): $(LEXER_SOURCE)
	@echo "Running Flex on $(LEXER_SOURCE)..."
	flex $(LEXER_SOURCE)

# Generate C code from parser specification
$(PARSER_OUTPUT) $(PARSER_HEADER): $(PARSER_SOURCE)
	@echo "Running Bison on $(PARSER_SOURCE)..."
	bison -d -o $(PARSER_OUTPUT) $(PARSER_SOURCE)

# Compile parse-tree library
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything into final compiler
$(TARGET): $(PARSER_OUTPUT) $(LEXER_OUTPUT) $(LIB_OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^ -lfl
	@echo "Build complete: $(TARGET)"

# Clean generated files
clean:
	@echo "Cleaning generated files..."
	rm -f $(LEXER_OUTPUT) $(PARSER_OUTPUT) $(PARSER_HEADER)
	rm -f $(LEXER_SOURCE) $(PARSER_SOURCE)
	rm -f $(LIB_OBJS)
	rm -f $(TARGET)
	@echo "Clean complete."

# Clean everything including backups
distclean: clean
	rm -f *.backup

# Rebuild from scratch
rebuild: clean all

# Detect input file based on DEF_FILE
# If DEF_FILE is in samples/, use corresponding input file, otherwise use test_input.txt
define get_input_file
$(if $(findstring samples/,$(1)),$(dir $(1))$(shell basename $(1) .def | sed 's/_analyzer/_input/').txt,test_input.txt)
endef

INPUT_FILE = $(call get_input_file,$(DEF_FILE))

# Run with colorful visualization (pipe through visualizer)
# First clean and rebuild to ensure we use the correct DEF_FILE
run: clean all
	@INPUT="$(INPUT_FILE)"; \
	if [ ! -f "$$INPUT" ]; then \
		echo "Error: Input file $$INPUT not found"; \
		echo "Expected input file based on $(DEF_FILE)"; \
		exit 1; \
	fi; \
	echo "Running $(TARGET) with colorful visualization..."; \
	echo "Input: $$INPUT"; \
	./$(TARGET) < "$$INPUT" | $(PYTHON) $(VISUALIZER_SCRIPT)

# Run with different visualization styles
run-simple: clean all
	@INPUT="$(INPUT_FILE)"; ./$(TARGET) < "$$INPUT" | $(PYTHON) $(VISUALIZER_SCRIPT) --style simple

run-compact: clean all
	@INPUT="$(INPUT_FILE)"; ./$(TARGET) < "$$INPUT" | $(PYTHON) $(VISUALIZER_SCRIPT) --style compact

run-stats: clean all
	@INPUT="$(INPUT_FILE)"; ./$(TARGET) < "$$INPUT" | $(PYTHON) $(VISUALIZER_SCRIPT) --stats

.PHONY: all clean distclean rebuild run run-simple run-compact run-stats
