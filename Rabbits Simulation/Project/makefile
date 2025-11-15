# === Variables ===
CC = gcc
CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -lm
SRC = TP4.c randomGenerator.c ../MT.c main.c
OBJ = $(SRC:.c=.o)
EXEC = main

# === Règles principales ===
all: $(EXEC) run plot

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation des .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Lancement du programme
run: $(EXEC)
	./$(EXEC)

# Lancement du script Python pour tracer les graphes
plot:
	python3 graphe.py

# Nettoyage
clean:
	rm -f $(OBJ) $(EXEC)

# Nettoyage complet + fichiers logs éventuels
mrproper: clean
	rm -f log.txt

.PHONY: all run plot clean mrproper
