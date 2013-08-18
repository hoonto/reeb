#ifndef CONTOURTREE_H_GUARD
#define CONTOURTREE_H_GUARD
#include <iostream>
#include "scalargraph.h"
#include "common.h"

class ContourTree {
    bool is_rooted;
    ScalarGraph contour_tree;
    ScalarGraph join_tree;
    ScalarGraph split_tree;
    void computeJoinTree(ScalarGraph&, std::vector<NodeID>&, std::map<NodeID,NodeID>&);
    void computeSplitTree(ScalarGraph&, std::vector<NodeID>&, std::map<NodeID,NodeID>&);
    void mergeTrees();
public:
    void computeAugmentedContourTree(ScalarGraph&);
    void computeContourTree(ScalarGraph&);
    void removeRegularVertices();
    bool isRegularNode(NodeID);
    std::ostream& prettyPrint(std::ostream&);
    bool isRooted();
    void chooseRoot(NodeID);
};


#endif
