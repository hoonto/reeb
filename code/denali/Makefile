CC = g++
IDIR = ./include
ODIR = ./build
SDIR = ./src
LEMONFLAGS = -Ideps/lemon/include -Ldeps/lemon/lib -lemon
CFLAGS = -I$(IDIR) $(LEMONFLAGS) -ggdb
COMMONDEPS = $(IDIR)/common.h

all:
	echo "Nothing to do..."

contour: contour.cpp $(ODIR)/scalargraph.o $(ODIR)/contourtree.o
	$(CC) $(CFLAGS) $(ODIR)/*.o contour.cpp -o contour

landscape: landscape.cpp $(ODIR)/scalargraph.o $(ODIR)/lscaperect.o
	$(CC) $(CFLAGS) $(ODIR)/*.o landscape.cpp -o landscape

$(ODIR)/scalargraph.o: $(SDIR)/scalargraph.cpp $(IDIR)/scalargraph.h $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/scalargraph.cpp -c -o $(ODIR)/scalargraph.o

$(ODIR)/contourtree.o: $(SDIR)/contourtree.cpp $(IDIR)/contourtree.h $(ODIR)/scalargraph.o $(ODIR)/disjointsetforest.o $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/contourtree.cpp -c -o $(ODIR)/contourtree.o

$(ODIR)/disjointsetforest.o: $(SDIR)/disjointsetforest.cpp $(IDIR)/disjointsetforest.h $(COMMONDEPS)
	$(CC) $(CFLAGS) $(SDIR)/disjointsetforest.cpp -c -o $(ODIR)/disjointsetforest.o

$(ODIR)/lscaperect.o: $(SDIR)/lscaperect.cpp $(IDIR)/lscaperect.h $(ODIR)/contourtree.o $(COMMONDEPS) 
	$(CC) $(CFLAGS) $(SDIR)/lscaperect.cpp -c -o $(ODIR)/lscaperect.o

clean:
	rm $(ODIR)/*.o
