#include "scalargraph.h"

void ScalarGraph::addNode(int id, double scalar_value) {
    // create a node in the graph
    Node n = g.addNode();
    // map to this node internally
    id_to_node[id] = n;
    value[n] = scalar_value;
}

void ScalarGraph::addEdge(int x, int y) {
    // get the nodes corresponding to x and y and add the edge
    g.addEdge(id_to_node[x], id_to_node[y]);
}

double ScalarGraph::getValue(int id) {
    return value[id_to_node[id]];
}


/*
void ScalarGraph::addNode(int id, double value) {
    nodes[id] = ScalarGraphNode();  
    nodes[id].value = value;
}

void ScalarGraph::addEdge(int x, int y) {
    nodes[x].neighbors.insert(y);
    nodes[y].neighbors.insert(x);
}

std::set<int> ScalarGraph::neighbors(int id) {
    return nodes[id].neighbors; 
}

double ScalarGraph::getValue(int id) {
    return nodes[id].value;
}

std::vector<int> ScalarGraph::getNodes() {
    std::vector<int> v;
    typedef std::map<int,ScalarGraphNode>::iterator iter;
    for (iter it=nodes.begin(); it!=nodes.end(); it++) {
        v.push_back(it->first);
    }
    return v;
}

std::vector<std::pair<int,double> > ScalarGraph::getNodeValuePairs() {
    std::vector<std::pair<int,double> > pairs;
    typedef std::map<int,ScalarGraphNode>::iterator iter;
    for (iter it=nodes.begin(); it!=nodes.end(); it++) {
        std::pair<int,double> p;
        p.first = it->first;
        p.second = (it->second).value;
        pairs.push_back(p);
    }
    return pairs;
}

std::vector<int> ScalarGraph::getSortedNodes() {
    std::vector<int> sorted_nodes = getNodes();
    std::sort(sorted_nodes.begin(), sorted_nodes.end(), ScalarGraph::node_sorting_functor(nodes));
    return sorted_nodes;
}

void ScalarGraph::prettyPrint(std::ostream& os) {
    std::vector<int> nodes = getSortedNodes();
    for (std::vector<int>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
        os << "Node " << (*it) << std::endl;
        os << "\tNeighbors: [ ";
        std::set<int> nbrs = neighbors(*it);
        std::copy(nbrs.begin(), nbrs.end(), std::ostream_iterator<int>(os, " "));
        os << "]" << std::endl;
        os << "\tValue: " << getValue(*it) << std::endl;
    }
}

void ScalarGraph::clear() {
    nodes.clear();
}
*/
