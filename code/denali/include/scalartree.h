#ifndef SCALARTREE_H_GUARD
#define SCALARTREE_H_GUARD
#include <lemon/list_graph.h>
#include <map>
#include <vector>
#include <iostream>
#include "common.h"

class ScalarTree {
private:
    typedef lemon::ListDigraph::Node Node;
    typedef lemon::ListDigraph::Arc Arc;
    lemon::ListDigraph g;
    lemon::ListDigraph::NodeMap<double> value;
    lemon::ListDigraph::NodeMap<NodeID> node_to_id;
    std::map<NodeID,Node> id_to_node;

public:
    ScalarTree() : value(g), node_to_id(g) { };
    void addNode(NodeID, double);
    double removeNode(NodeID);
    void addEdge(NodeID, NodeID);
    void removeEdge(NodeID, NodeID);
    double getValue(NodeID);
    std::list<NodeID> getNeighbors(NodeID);
    std::list<NodeID> getChildren(NodeID);
    std::list<NodeID> getPredecessors(NodeID);
    unsigned int getNumberOfNeighbors(NodeID);
    unsigned int getNumberOfChildren(NodeID);
    unsigned int getNumberOfPredecessors(NodeID);
    std::vector<NodeID> getNodes();
    std::vector<NodeID> getSortedNodes();
    void prettyPrint(std::ostream& os);
    void clear();
};

#endif
