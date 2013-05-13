CC=gcc
CFLAGS= -Wall -Wextra -Werror -pedantic

LIBS=-lrt -lm

DEPS = ReadFile.h ReadDirectory.h Sets.h BeliefFunctions.h BeliefsFromSensors.h BeliefsFromBeliefs.h BeliefsFromRandomness.h Tests.h config.h
OBJ = ReadFile.o ReadDirectory.o Sets.o BeliefFunctions.o BeliefsFromSensors.o BeliefsFromBeliefs.o BeliefsFromRandomness.o Tests.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Exe_BF: $(OBJ)
	gcc -O3 -o $@ $^ $(CFLAGS) $(LIBS)
	rm *.o

clean:
	rm $(OBJ)
	
libTHEGAME.a: $(OBJ)
	$(AR) $(ARFLAGS) $@ $?
