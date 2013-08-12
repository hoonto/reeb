#ifndef DISJOINTSETFOREST_H_GUARD
#define DISJOINTSETFOREST_H_GUARD

#include <map>

class DisjointSetForest {
public:
    struct DisjointSet {
        int parent;
        int rank;
        int max;
        int min;
    };

    std::map<int,DisjointSet> id_to_set;

public:
    void makeSet(int);
    void unionSets(int,int,bool(int,int));
    int findSet(int);
    int maxSet(int);
    int minSet(int);
};

#endif
