CC = g++ -std=c++11
CCFLAGS = -c

all: ensemble linear voter

ensemble: Ensemble.o commons.h
	$(CC) Ensemble.o -o EnsembleClassifier.out

linear: Linear.o commons.h
	$(CC) Linear.o -o Linear.out

voter: Voter.o commons.h
	$(CC) Voter.o -o Voter.out

Ensemble.o: Ensemble.cpp commons.h
	$(CC) $(CCFLAGS) Ensemble.cpp

Linear.o: Linear.cpp commons.h
	$(CC) $(CCFLAGS) Linear.cpp

Voter.o: Voter.cpp commons.h
	$(CC) $(CCFLAGS) Voter.cpp

clean:
	rm -r *.o