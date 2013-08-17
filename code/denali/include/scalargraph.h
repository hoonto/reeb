#ifndef SCALARGRAPH_H_GUARD
#define SCALARGRAPH_H_GUARD
#include <lemon/list_graph.h>
#include <lemon/adaptors.h>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include "common.h"


class ScalarGraph {
private:
    typedef lemon::ListDigraph::Node Node;
    typedef lemon::ListDigraph::Arc Arc;
    lemon::ListDigraph g;
    lemon::ListDigraph::NodeMap<double> value;
    lemon::ListDigraph::NodeMap<NodeID> node_to_id;
    std::map<NodeID,Node> id_to_node;
    lemon::ListDigraph::ArcMap<std::set<NodeID> > edge_members;
public:
    ScalarGraph() : 
        value(g), node_to_id(g), edge_members(g) { };


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

    std::set<NodeID>& edgeMembers(NodeID, NodeID);

    std::vector<NodeID> getNodes();
    std::vector<NodeID> getSortedNodes();
    void prettyPrint(std::ostream& os);
    void clear();


};

#endif
