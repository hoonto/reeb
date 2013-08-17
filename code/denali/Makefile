CC = g++
IDIR = ./include
ODIR = ./build
SDIR = ./src
LEMONFLAGS = -Ideps/lemon/include -Ldeps/lemon/lib -lemon
CFLAGS = -I$(IDIR) $(LEMONFLAGS) -ggdb
COMMONDEPS = $(IDIR)/common.h

all:
	echo "Nothing to do..."

contour: contour.cpp scalargraph contourtree
	$(CC) $(CFLAGS) $(ODIR)/*.o contour.cpp -o contour

scalargraph: $(SDIR)/scalargraph.cpp $(IDIR)/scalargraph.h $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/scalargraph.cpp -c -o $(ODIR)/scalargraph.o

contourtree: $(SDIR)/contourtree.cpp $(IDIR)/contourtree.h scalargraph disjointsetforest $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/contourtree.cpp -c -o $(ODIR)/contourtree.o

disjointsetforest: $(SDIR)/disjointsetforest.cpp $(IDIR)/disjointsetforest.h $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/disjointsetforest.cpp -c -o $(ODIR)/disjointsetforest.o

clean:
	rm $(ODIR)/*.o
