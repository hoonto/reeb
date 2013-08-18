#ifndef SCALARGRAPH_H_GUARD
#define SCALARGRAPH_H_GUARD
#include <lemon/list_graph.h>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include "common.h"


class ScalarGraph {
private:
    typedef lemon::ListDigraph::Node Node;
    typedef lemon::ListDigraph::Arc Arc;
    lemon::ListDigraph g;
    lemon::ListDigraph::NodeMap<double> value;
    lemon::ListDigraph::NodeMap<NodeID> node_to_id;
    std::map<NodeID,Node> id_to_node;
public:
    ScalarGraph() : 
        value(g), node_to_id(g) { };

    void addNode(NodeID, double);
    double removeNode(NodeID);
    bool hasNode(NodeID);
    double getValue(NodeID);

    void addEdge(NodeID, NodeID);
    void removeEdge(NodeID, NodeID);

    std::list<NodeID> getChildren(NodeID);
    std::list<NodeID> getPredecessors(NodeID);
    unsigned int getNumberOfChildren(NodeID);
    unsigned int getNumberOfPredecessors(NodeID);

    std::list<NodeID> getNeighbors(NodeID);
    unsigned int getNumberOfNeighbors(NodeID);

    std::vector<NodeID> getNodes();
    std::vector<NodeID> getSortedNodes();
    std::ostream& prettyPrint(std::ostream&);
    void clear();

    std::map<NodeID,NodeID> dfsPredecessorMap(NodeID);

    class UndirectedEdge;

};


class ScalarGraph::UndirectedEdge {
    NodeID small;
    NodeID big;
public:
    UndirectedEdge(NodeID u, NodeID v) { small = std::min(u,v); big = std::max(u,v); }
    NodeID u() const { return small; }
    NodeID v() const { return big; }

    bool operator<(const ScalarGraph::UndirectedEdge& other) const {
        NodeID x_u = u();
        NodeID y_u = other.u();
        NodeID x_v = v();
        NodeID y_v = other.v();
        return (x_u < y_u) || ((x_u == y_u) && (x_v < y_v));
    }
};

#endif
