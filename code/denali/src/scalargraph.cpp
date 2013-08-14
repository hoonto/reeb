#include <algorithm>
#include <iterator>
#include "scalargraph.h"


void ScalarGraph::addNode(NodeID id, double scalar_value) {
    // create a node in the graph
    Node n = g.addNode();
    value[n] = scalar_value;
    // map to this node internally
    id_to_node[id] = n;
    node_to_id[n] = id;
}


double ScalarGraph::removeNode(NodeID id) {
    double node_value = value[id_to_node[id]];
    // remove the node from the graph
    g.erase(id_to_node[id]);
    // remove the node from the manual mapping
    id_to_node.erase(id);
    return node_value;
}


void ScalarGraph::addEdge(NodeID x, NodeID y) {
    // get the nodes corresponding to x and y and add the edge
    g.addEdge(id_to_node[x], id_to_node[y]);
}


void ScalarGraph::removeEdge(NodeID x, NodeID y) {
    Edge e = lemon::findEdge(g, id_to_node[x], id_to_node[y]);
    g.erase(e);
}


double ScalarGraph::getValue(NodeID id) {
    return value[id_to_node[id]];
}


unsigned int ScalarGraph::getNumberOfNeighbors(NodeID id) {
    return lemon::countIncEdges(g, id_to_node[id]);
}


std::list<NodeID> ScalarGraph::getNeighbors(NodeID id) {
    std::list<NodeID> neighbors;
    for (lemon::ListGraph::IncEdgeIt e(g, id_to_node[id]); e!=lemon::INVALID; ++e) {
        neighbors.push_back(node_to_id[g.oppositeNode(id_to_node[id], e)]);
    }
    return neighbors;
}


std::vector<NodeID> ScalarGraph::getNodes() {
    std::vector<NodeID> nodes;
    for (lemon::ListGraph::NodeIt n(g); n!=lemon::INVALID; ++n) {
        nodes.push_back(node_to_id[n]);
    }
    return nodes;
}


struct node_sorter {
private:
    ScalarGraph* g;
public:
    node_sorter(ScalarGraph* x) {g=x;}
    bool operator()(NodeID x, NodeID y) { return g->getValue(x) < g->getValue(y); }
};


std::vector<NodeID> ScalarGraph::getSortedNodes() {
    // first we get the nodes
    std::vector<NodeID> nodes = getNodes();
    // in order to sort the nodes, we need to use a functor that contains values
    std::sort(nodes.begin(), nodes.end(), node_sorter(this));
    return nodes;
}


void ScalarGraph::prettyPrint(std::ostream& os) {
    for (lemon::ListGraph::NodeIt v(g); v!=lemon::INVALID; ++v) {
        os << "Node " << node_to_id[v] << std::endl;
        os << "\tValue: " << value[v] << std::endl;
        os << "\tNeighbors: [ ";
        std::list<NodeID> neighbors = getNeighbors(node_to_id[v]);
        std::copy(neighbors.begin(), neighbors.end(), std::ostream_iterator<NodeID>(os," "));
        os << "]" << std::endl;
    }
}

void ScalarGraph::clear() {
    g.clear();
    id_to_node.clear();
}
