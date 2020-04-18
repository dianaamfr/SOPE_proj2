.PHONY: all begin generate run clean

all: begin generate

begin:
		@echo "## Starting Compiling ##"

generate:
		@echo "Creating ouput files..."
		gcc main.c logging.c -o test -Wall -lpthread -D_REENTRANT

run: begin generate
		@echo "## Starting running ##"
		./test

log: begin generate
		./test > ../log/log.txt

clean:
		@echo "Cleaning up..."
		-rm test -f
