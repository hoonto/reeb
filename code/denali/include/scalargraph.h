#ifndef SCALARGRAPH_H_GUARD
#define SCALARGRAPH_H_GUARD
#include <lemon/list_graph.h>
#include <lemon/adaptors.h>
#include <map>
#include <vector>
#include <iostream>
#include "common.h"

/*
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
    std::vector<std::vector<NodeID> > getEdges();
    unsigned int getNumberOfNeighbors(NodeID);
    std::vector<NodeID> getNodes();
    std::vector<NodeID> getSortedNodes();
    void prettyPrint(std::ostream& os);
    void clear();

    friend class ScalarGraphDirectedView;
};
*/


class ScalarGraph {
private:
    typedef lemon::ListDigraph::Node Node;
    typedef lemon::ListDigraph::Arc Arc;
    lemon::ListDigraph g;
    lemon::ListDigraph::NodeMap<double> value;
    lemon::ListDigraph::NodeMap<NodeID> node_to_id;
    std::map<NodeID,Node> id_to_node;
    lemon::ListDigraph::ArcMap<double> edge_weight;
public:
    ScalarGraph() : 
        value(g), node_to_id(g), edge_weight(g,0) { };


    void addNode(NodeID, double);
    double removeNode(NodeID);
    bool hasNode(NodeID);
    double getValue(NodeID);

    void addEdge(NodeID, NodeID);
    void removeEdge(NodeID, NodeID);
    double getEdgeWeight(NodeID, NodeID);
    void setEdgeWeight(NodeID, NodeID, double);

    std::list<NodeID> getChildren(NodeID);
    std::list<NodeID> getPredecessors(NodeID);
    unsigned int getNumberOfChildren(NodeID);
    unsigned int getNumberOfPredecessors(NodeID);

    std::list<NodeID> getNeighbors(NodeID);
    unsigned int getNumberOfNeighbors(NodeID);

    std::vector<NodeID> getNodes();
    std::vector<NodeID> getSortedNodes();
    void prettyPrint(std::ostream& os);
    void clear();


};

#endif
