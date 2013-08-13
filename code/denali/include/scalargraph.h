#ifndef SCALARGRAPH_H_GUARD
#define SCALARGRAPH_H_GUARD
#include <lemon/list_graph.h>
#include <map>

class ScalarGraph {
private:
    typedef lemon::ListGraph::Node Node;
    typedef lemon::ListGraph::Arc Arc;
    typedef lemon::ListGraph::Edge Edge;
    lemon::ListGraph g;
    lemon::ListGraph::NodeMap<double> value;
    std::map<int,Node> id_to_node;
public:
    ScalarGraph() : value(g) { };
    void addNode(int, double);
    void addEdge(int, int);
    double getValue(int);
    std::list<int> neighbors(int);
};

/*
class ScalarGraph {
protected:
    class ScalarGraphNode {
    public:
        double value;
        std::set<int> neighbors;
    };

    // map holding all of the nodes
    std::map<int,ScalarGraphNode> nodes;

public:
    typedef std::vector<int>::iterator node_iter_type;
    typedef std::vector<std::pair<int,double> >::iterator node_value_pair_iter_type;
    void addNode(int, double);
    void addEdge(int, int);
    std::set<int> neighbors(int);
    double getValue(int);
    std::vector<int> getNodes();
    std::vector<std::pair<int,double> > getNodeValuePairs();
    std::vector<int> getSortedNodes();
    void prettyPrint(std::ostream& os);
    void clear();

    // A fancy way to sort the nodes
    struct node_sorting_functor {
        node_sorting_functor(std::map<int,ScalarGraphNode>& n) : n(n) {}
        bool operator()(int x, int y) { return n[x].value<n[y].value; }
        private:
        std::map<int,ScalarGraphNode>& n;
    };
};
*/

#endif
