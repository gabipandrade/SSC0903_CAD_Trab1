# Compilation settings
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2 -fopenmp
LDLIBS = -lm

# Executable files
EXE_SEQ = studentsseq
EXE_PAR = studentspar

# Source files
SRC = studentsseq.c studentspar.c

# Object files
OBJ = $(SRC:.c=.o)

TARGETS = $(SRC:.c=)

.PHONY: all run_seq run_par clean

# Rules
all: $(TARGETS)

$(TARGETS): %: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run_seq: $(EXE_SEQ)
	./$(EXE_SEQ)

run_par: $(EXE_PAR)
	./$(EXE_PAR)

clean:
	rm -f $(TARGETS) *.o
