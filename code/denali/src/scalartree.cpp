#include <algorithm>
#include <iterator>
#include "scalartree.h"


void ScalarTree::addNode(NodeID id, double scalar_value) {
    // create a node in the graph
    Node n = g.addNode();
    value[n] = scalar_value;
    // map to this node internally
    id_to_node[id] = n;
    node_to_id[n] = id;
}


double ScalarTree::removeNode(NodeID id) {
    double node_value = value[id_to_node[id]];
    // remove the node from the graph
    g.erase(id_to_node[id]);
    // remove the node from the manual mapping
    id_to_node.erase(id);
    return node_value;
}


void ScalarTree::addEdge(NodeID x, NodeID y) {
    // get the nodes corresponding to x and y and add the edge
    g.addArc(id_to_node[x], id_to_node[y]);
}


void ScalarTree::removeEdge(NodeID x, NodeID y) {
    Arc a = lemon::findArc(g, id_to_node[x], id_to_node[y]);
    g.erase(a);
}


double ScalarTree::getValue(NodeID id) {
    return value[id_to_node[id]];
}


std::vector<NodeID> ScalarTree::getNodes() {
    std::vector<NodeID> nodes;
    for (lemon::ListDigraph::NodeIt n(g); n!=lemon::INVALID; ++n) {
        nodes.push_back(node_to_id[n]);
    }
    return nodes;
}


struct node_sorter {
private:
    ScalarTree* g;
public:
    node_sorter(ScalarTree* x) {g=x;}
    bool operator()(NodeID x, NodeID y) { return g->getValue(x) < g->getValue(y); }
};


std::vector<NodeID> ScalarTree::getSortedNodes() {
    // first we get the nodes
    std::vector<NodeID> nodes = getNodes();
    // in order to sort the nodes, we need to use a functor that contains values
    std::sort(nodes.begin(), nodes.end(), node_sorter(this));
    return nodes;
}


std::list<NodeID> ScalarTree::getChildren(NodeID id) {
    std::list<NodeID> children;
    Node u = id_to_node[id];
    for (lemon::ListDigraph::OutArcIt a(g,u); a!=lemon::INVALID; ++a) {
        Node v = g.oppositeNode(u,a);
        children.push_back(node_to_id[v]);
    }
    return children;
}


std::list<NodeID> ScalarTree::getPredecessors(NodeID id) {
    std::list<NodeID> predecessors;
    Node u = id_to_node[id];
    for (lemon::ListDigraph::InArcIt a(g,u); a!=lemon::INVALID; ++a) {
        Node v = g.oppositeNode(u,a);
        predecessors.push_back(node_to_id[v]);
    }
    return predecessors;
}


std::list<NodeID> ScalarTree::getNeighbors(NodeID id) {
    std::list<NodeID> neighbors = getChildren(id);
    std::list<NodeID> predecessors = getPredecessors(id);
    neighbors.insert(neighbors.end(), predecessors.begin(), predecessors.end());
    return neighbors;
}


unsigned int ScalarTree::getNumberOfChildren(NodeID id) {
    return lemon::countOutArcs(g, id_to_node[id]); 
}


unsigned int ScalarTree::getNumberOfPredecessors(NodeID id) {
    return lemon::countInArcs(g, id_to_node[id]); 
}


unsigned int ScalarTree::getNumberOfNeighbors(NodeID id) {
    return getNumberOfChildren(id) + getNumberOfPredecessors(id);
}


void ScalarTree::prettyPrint(std::ostream& os) {
    for (lemon::ListDigraph::NodeIt v(g); v!=lemon::INVALID; ++v) {
        os << "Node " << node_to_id[v] << std::endl;
        os << "\tValue: " << value[v] << std::endl;

        std::list<NodeID> children = getChildren(node_to_id[v]);
        std::list<NodeID> predecessors = getPredecessors(node_to_id[v]);
        std::list<NodeID> neighbors = getNeighbors(node_to_id[v]);

        os << "\t# Children: " << getNumberOfChildren(node_to_id[v]);
        os << "\t# Predecessors: " << getNumberOfPredecessors(node_to_id[v]);
        os << "\t# Neighbors: " << getNumberOfNeighbors(node_to_id[v]);
        os << std::endl;

        os << "\tChildren: [ ";
        std::copy(children.begin(), children.end(), std::ostream_iterator<int>(os, " "));
        os << "]" << std::endl;

        os << "\tPredecessors: [ ";
        std::copy(predecessors.begin(), predecessors.end(), std::ostream_iterator<int>(os, " "));
        os << "]" << std::endl;

        os << "\tNeighbors: [ ";
        std::copy(neighbors.begin(), neighbors.end(), std::ostream_iterator<int>(os, " "));
        os << "]" << std::endl;
    }
}


void ScalarTree::clear() {
    g.clear();
    id_to_node.clear();
}
