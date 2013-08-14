#ifndef CONTOURTREE_H_GUARD
#define CONTOURTREE_H_GUARD
#include "scalargraph.h"

class ContourTree : public ScalarGraph {
    public:
    void compute(ScalarGraph& sg);

    private:
    void computeJoinTree(ScalarGraph&, std::vector<NodeID>&, std::map<NodeID,NodeID>&);
    void computeSplitTree(ScalarGraph&, std::vector<NodeID>&, std::map<NodeID,NodeID>&);
    void mergeTrees();
    ScalarTree join_tree;
    ScalarTree split_tree;
};

#endif
