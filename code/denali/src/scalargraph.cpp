#include <algorithm>
#include <iterator>
#include <lemon/adaptors.h>
#include <lemon/dfs.h>
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
    // Adds an edge from x to y. If:
    //  - an arc from x to y already exists, nothing happens
    //  - an arc from y to x exists, it is flipped
    Node nx = id_to_node[x];
    Node ny = id_to_node[y];
    Arc a1 = lemon::findArc(g, nx, ny);
    Arc a2 = lemon::findArc(g, ny, nx);
    if (a1==lemon::INVALID && a2==lemon::INVALID) {
        g.addArc(id_to_node[x], id_to_node[y]);
    } else if (a1==lemon::INVALID) {
        g.reverseArc(a2);
    }
}


void ScalarGraph::removeEdge(NodeID x, NodeID y) {
    Arc e = lemon::findArc(g, id_to_node[x], id_to_node[y]);
    if (e == lemon::INVALID)
        e = lemon::findArc(g, id_to_node[y], id_to_node[x]);

    g.erase(e);
}


std::set<NodeID>& ScalarGraph::edgeMembers(NodeID x, NodeID y) {
    Arc e = lemon::findArc(g, id_to_node[x], id_to_node[y]);
    if (e == lemon::INVALID)
        e = lemon::findArc(g, id_to_node[y], id_to_node[x]);

    return edge_members[e];
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


std::ostream& ScalarGraph::prettyPrint(std::ostream& os) {
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
    return os;
}


std::map<NodeID,NodeID> ScalarGraph::dfsPredecessorMap(NodeID source) {
    // Given a source node, computes a DFS tree, returning a mapping from each
    // node to its predecessor.

    // look at the undirected version of the graph
    lemon::Undirector<lemon::ListDigraph> g_undirected(g);

    // compute a NodeMap of predecessors by doing a DFS
    lemon::ListDigraph::NodeMap<Arc> arcmap(g);
    lemon::dfs(g_undirected).predMap(arcmap).run(id_to_node[source]);

    // now process the arcs into a map from nodeID to predecessor
    std::map<NodeID,NodeID> predmap;
    for (lemon::ListDigraph::NodeIt it(g); it!=lemon::INVALID; ++it) {
        Arc a = arcmap[it];
        Node other = g.oppositeNode(it, a);
        predmap[node_to_id[it]] = node_to_id[other];
    }
    return predmap;
}
