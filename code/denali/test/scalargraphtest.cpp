#include <iostream>
#include <algorithm>
#include <iterator>
#include "scalargraph.h"

int main() {
    using std::cout;
    using std::endl;

    ScalarGraph g;
    int n_nodes = 10;

    for (int i=0; i<n_nodes; ++i) {
        g.addNode(i, (double) i);
    }

    g.addEdge(0,2);
    g.addEdge(2,4);
    g.addEdge(0,2);

    for (int i=0; i<n_nodes; ++i) {
        cout << "Node " << i << ":" << endl;
        cout << "\tValue = " << g.getValue(i);
        cout << "\tNeighbors = ";
        std::set<int> n = g.neighbors(i);
        cout << "[ ";
        std::copy(n.begin(), n.end(), std::ostream_iterator<int>(cout, " "));
        cout << "]";
        cout << endl;
    }


}
