#ifndef CONTOURTREE_H_GUARD
#define CONTOURTREE_H_GUARD
#include <iostream>
#include "scalargraph.h"
#include "common.h"


class ContourTree {
    bool is_rooted;
    NodeID root;
    ScalarGraph join_tree;
    ScalarGraph split_tree;

    typedef ScalarGraph::UndirectedEdge Edge;
    std::map<NodeID,unsigned int> total_node_weight;
    std::map<Edge,unsigned int> total_edge_weight;

    void computeJoinTree(ScalarGraph&, std::vector<NodeID>&, std::map<NodeID,NodeID>&);
    void computeSplitTree(ScalarGraph&, std::vector<NodeID>&, std::map<NodeID,NodeID>&);
    void mergeTrees();

    void updateTotalWeights(NodeID);

public:
    std::map<Edge,std::set<NodeID> > edge_members;
    ScalarGraph contour_tree;
    ContourTree(){};

    void computeAugmentedContourTree(ScalarGraph&);
    void computeContourTree(ScalarGraph&);
    void removeRegularVertices();
    bool isRegularNode(NodeID);

    bool isRooted();
    void setRoot(NodeID);
    NodeID getRoot();

    unsigned int edgeWeight(NodeID x, NodeID y);

    unsigned int totalNodeWeight(NodeID);
    unsigned int totalEdgeWeight(NodeID,NodeID);

    //
    // SCALAR GRAPH INTERFACE
    //

    bool hasNode(NodeID x) 
        { return contour_tree.hasNode(x); }

    double getValue(NodeID x) 
        { return contour_tree.getValue(x); }

    std::list<NodeID> getChildren(NodeID x) 
        { return contour_tree.getChildren(x); }

    std::list<NodeID> getPredecessors(NodeID x)
        { return contour_tree.getPredecessors(x); }

    unsigned int getNumberOfChildren(NodeID x)
        { return contour_tree.getNumberOfChildren(x); }

    unsigned int getNumberOfPredecessors(NodeID x)
        { return contour_tree.getNumberOfPredecessors(x); }

    std::list<NodeID> getNeighbors(NodeID x)
        { return contour_tree.getNeighbors(x); }

    unsigned int getNumberOfNeighbors(NodeID x)
        { return contour_tree.getNumberOfNeighbors(x); }

    std::vector<NodeID> getNodes()
        { return contour_tree.getNodes(); }

    std::vector<NodeID> getSortedNodes()
        { return contour_tree.getSortedNodes(); }

    std::ostream& prettyPrint(std::ostream& os)
        { return contour_tree.prettyPrint(os); }

    std::map<NodeID,NodeID> dfsPredecessorMap(NodeID x)
        { return contour_tree.dfsPredecessorMap(x); }
};

#endif
