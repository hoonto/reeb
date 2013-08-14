#ifndef DISJOINTSETFOREST_H_GUARD
#define DISJOINTSETFOREST_H_GUARD
#include <map>
#include "common.h"

class DisjointSetForest {
public:
    struct DisjointSet {
        NodeID parent;
        NodeID rank;
        NodeID max;
        NodeID min;
    };

    std::map<NodeID,DisjointSet> id_to_set;

public:
    void makeSet(NodeID);
    void unionSets(NodeID,NodeID,std::map<NodeID,NodeID>);
    NodeID findSet(NodeID);
    NodeID maxSet(NodeID);
    NodeID minSet(NodeID);
};

#endif
