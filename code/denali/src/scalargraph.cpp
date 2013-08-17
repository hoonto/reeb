#include <algorithm>
#include <iterator>
#include "scalargraph.h"


void ScalarGraph::addNode(NodeID id, double scalar_value) {
    // create a node in the graph, assuming it doesn't already exist
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


bool ScalarGraph::hasNode(NodeID id) {
    return id_to_node.find(id) != id_to_node.end();
}


double ScalarGraph::getValue(NodeID id) {
    return value[id_to_node[id]];
}


void ScalarGraph::addEdge(NodeID x, NodeID y) {
    // get the nodes corresponding to x and y and add the edge (only if edge does not
    // already exist)
    if (lemon::findArc(g, id_to_node[x], id_to_node[y]) == lemon::INVALID &&
        lemon::findArc(g, id_to_node[y], id_to_node[x]) == lemon::INVALID)
        g.addArc(id_to_node[x], id_to_node[y]);
}


void ScalarGraph::removeEdge(NodeID x, NodeID y) {
    Arc e = lemon::findArc(g, id_to_node[x], id_to_node[y]);
    if (e == lemon::INVALID)
        e = lemon::findArc(g, id_to_node[y], id_to_node[x]);

    g.erase(e);
}


double ScalarGraph::getEdgeWeight(NodeID x, NodeID y) {
    Arc e = lemon::findArc(g, id_to_node[x], id_to_node[y]);
    if (e == lemon::INVALID)
        e = lemon::findArc(g, id_to_node[y], id_to_node[x]);
    return edge_weight[e];
}


void ScalarGraph::setEdgeWeight(NodeID x, NodeID y, double value) {
    Arc e = lemon::findArc(g, id_to_node[x], id_to_node[y]);
    if (e == lemon::INVALID)
        e = lemon::findArc(g, id_to_node[y], id_to_node[x]);
    edge_weight[e] = value;
}


std::list<NodeID> ScalarGraph::getChildren(NodeID id) {
    std::list<NodeID> children;
    Node u = id_to_node[id];
    for(lemon::ListDigraph::OutArcIt a(g,u); a!=lemon::INVALID; ++a) {
        Node v = g.oppositeNode(u,a);
        children.push_back(node_to_id[v]);
    }
    return children;
}


std::list<NodeID> ScalarGraph::getPredecessors(NodeID id) {
    std::list<NodeID> predecessors;
    Node u = id_to_node[id];
    for(lemon::ListDigraph::InArcIt a(g,u); a!=lemon::INVALID; ++a) {
        Node v = g.oppositeNode(u,a);
        predecessors.push_back(node_to_id[v]);
    }
    return predecessors;
}


unsigned int ScalarGraph::getNumberOfChildren(NodeID id) {
    return countOutArcs(g, id_to_node[id]); 
}


unsigned int ScalarGraph::getNumberOfPredecessors(NodeID id) {
    return countInArcs(g, id_to_node[id]); 
}


std::list<NodeID> ScalarGraph::getNeighbors(NodeID id) {
    std::list<NodeID> children = getChildren(id);
    std::list<NodeID> predecessors = getPredecessors(id);
    children.insert(children.end(), predecessors.begin(), predecessors.end());
    return children;
}


unsigned int ScalarGraph::getNumberOfNeighbors(NodeID id) {
    return getNumberOfChildren(id) + getNumberOfPredecessors(id);
}


std::vector<NodeID> ScalarGraph::getNodes() {
    std::vector<NodeID> nodes;
    for (lemon::ListDigraph::NodeIt n(g); n!=lemon::INVALID; ++n) {
        nodes.push_back(node_to_id[n]);
    }
    return nodes;
}


namespace {
struct node_sorter {
private:
    ScalarGraph* g;
public:
    node_sorter(ScalarGraph* x) {g=x;}
    bool operator()(NodeID x, NodeID y) { return g->getValue(x) < g->getValue(y); }
};
}


std::vector<NodeID> ScalarGraph::getSortedNodes() {
    // first we get the nodes
    std::vector<NodeID> nodes = getNodes();
    // in order to sort the nodes, we need to use a functor that contains values
    std::sort(nodes.begin(), nodes.end(), node_sorter(this));
    return nodes;
}


void ScalarGraph::clear() {
    g.clear();
    id_to_node.clear();
}


void ScalarGraph::prettyPrint(std::ostream& os) {
    for (lemon::ListDigraph::NodeIt v(g); v!=lemon::INVALID; ++v) {
        os << "Node " << node_to_id[v] << std::endl;
        os << "\tValue: " << value[v] << std::endl;

        std::ostream_iterator<NodeID> vector_printer(os, " ");

        os << "\tChildren: [ ";
        std::list<NodeID> children = getChildren(node_to_id[v]);
        std::copy(children.begin(), children.end(), vector_printer);
        os << "]" << std::endl;

        os << "\tPredecessors: [ ";
        std::list<NodeID> predecessors = getPredecessors(node_to_id[v]);
        std::copy(predecessors.begin(), predecessors.end(), vector_printer);
        os << "]" << std::endl;

        os << "\tNeighbors: [ ";
        std::list<NodeID> neighbors = getNeighbors(node_to_id[v]);
        std::copy(neighbors.begin(), neighbors.end(), vector_printer);
        os << "]" << std::endl;
    }
}
