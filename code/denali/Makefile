CC = g++
IDIR = ./include
ODIR = ./build
SDIR = ./src
LEMONFLAGS = -Ideps/lemon/include -Ldeps/lemon/lib -lemon
CFLAGS = -I$(IDIR) $(LEMONFLAGS)
COMMONDEPS = $(IDIR)/common.h

all: contour

contour: contour.cpp $(ODIR)/scalargraph.o $(ODIR)/scalartree.o \
$(ODIR)/disjointsetforest.o $(ODIR)/contourtree.o
	$(CC) $(CFLAGS) $(ODIR)/*.o contour.cpp -o main

$(ODIR)/scalargraph.o: $(SDIR)/scalargraph.cpp $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/scalargraph.cpp -c -o $(ODIR)/scalargraph.o

$(ODIR)/scalartree.o: $(SDIR)/scalartree.cpp $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/scalartree.cpp -c -o $(ODIR)/scalartree.o

$(ODIR)/disjointsetforest.o: $(SDIR)/disjointsetforest.cpp $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/disjointsetforest.cpp -c -o $(ODIR)/disjointsetforest.o

$(ODIR)/contourtree.o: $(SDIR)/contourtree.cpp $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/contourtree.cpp -c -o $(ODIR)/contourtree.o

clean:
	rm $(ODIR)/*.o
