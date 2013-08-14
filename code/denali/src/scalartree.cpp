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


std::list<NodeID> ScalarTree::getNodes() {
    std::list<NodeID> nodes;
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


std::list<NodeID> ScalarTree::getSortedNodes() {
    // first we get the nodes
    std::list<NodeID> nodes = getNodes();
    // in order to sort the nodes, we need to use a functor that contains values
    nodes.sort(node_sorter(this));
    return nodes;
}


/*
void ScalarTree::prettyPrint(std::ostream& os) {
    for (lemon::ListDigraph::NodeIt v(g); v!=lemon::INVALID; ++v) {
        os << "Node " << node_to_id[v] << std::endl;
        os << "\tValue: " << value[v] << std::endl;
        os << "\tNeighbors: [ ";
        std::list<NodeID> neighbors = getNeighbors(node_to_id[v]);
        std::copy(neighbors.begin(), neighbors.end(), std::ostream_iterator<NodeID>(os," "));
        os << "]" << std::endl;
    }
}
*/
