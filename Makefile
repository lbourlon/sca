LIBS    = "-lm"
CC 		= "gcc"


DEBUG_CFLAGS = -Wall -g -O0
CFLAGS = -O3

all: main.c pearson.c matrice.c ingest.c others.c attacks.c
	$(CC) $(CFLAGS) -o prog main.c  matrice.c pearson.c ingest.c others.c attacks.c  $(LIBS)

debug: main.c pearson.c matrice.c ingest.c others.c attacks.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o prog main.c  matrice.c pearson.c ingest.c others.c attacks.c  $(LIBS)