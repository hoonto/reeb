#ifndef SCALARGRAPH_H_GUARD
#define SCALARGRAPH_H_GUARD
#include <lemon/list_graph.h>
#include <map>
#include <vector>
#include <iostream>
#include "common.h"

class ScalarGraph {
private:
    typedef lemon::ListGraph::Node Node;
    typedef lemon::ListGraph::Edge Edge;
    lemon::ListGraph g;
    lemon::ListGraph::NodeMap<double> value;
    lemon::ListGraph::NodeMap<NodeID> node_to_id;
    std::map<NodeID,Node> id_to_node;

public:
    ScalarGraph() : value(g), node_to_id(g) { };
    void addNode(NodeID, double);
    double removeNode(NodeID);
    void addEdge(NodeID, NodeID);
    void removeEdge(NodeID, NodeID);
    double getValue(NodeID);
    std::list<NodeID> getNeighbors(NodeID);
    unsigned int getNumberOfNeighbors(NodeID);
    std::vector<NodeID> getNodes();
    std::vector<NodeID> getSortedNodes();
    void prettyPrint(std::ostream& os);
    void clear();
};

#endif
