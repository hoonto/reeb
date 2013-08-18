#include <iostream>
#include <iterator>
#include <string>
#include "scalargraph.h"
#include "contourtree.h"

int main() {

    // make the scalar graph on pg. 390 of Wenger
    ScalarGraph g;

    const int nodevalues[] = {25, 62, 45, 66, 16, 32, 64, 39, 58, 51, 53, 30};
    for (int i=0; i<12; ++i) {
        g.addNode(i+1, nodevalues[i]);
    }

    const int edges[][2] = {{1,2}, {2,3}, {4,5}, {5,6}, {7,8}, {8,9}, {10,11}, {11,12},
                            {1,4}, {4,7}, {7,10}, {2,5}, {5,8}, {8,11}, {3,6}, {6,9}, {9,12},
                            {1,5}, {2,6}, {4,8}, {5,9}, {7,11}, {8,12}};

    for (int i=0; i<sizeof(edges)/sizeof(edges[0]); ++i) {
        g.addEdge(edges[i][0], edges[i][1]);
    }

    ScalarGraph x;
    x.addNode(0,0);
    x.addNode(1,0);
    x.addNode(2,0);
    x.addNode(3,0);
    x.addNode(4,0);
    x.addNode(5,0);

    x.addEdge(1,0);
    x.addEdge(4,0);
    x.addEdge(1,2);
    x.addEdge(3,1);
    x.addEdge(2,5);

    x.prettyPrint(std::cout);

    std::map<NodeID,NodeID> pm = x.dfsPredecessorMap(0);

    for (int i=0; i<=5; ++i) {
        std::cout << "Node: " << i << " Pred: " << pm[i] << std::endl;
    }

    /*
    ScalarGraph act;
    ScalarGraph ct;
    denali::computeAugmentedContourTree(g,act);
    denali::computeContourTree(g,ct);

    act.prettyPrint(std::cout);
    std::cout << std::endl << std::string(80, '=') << "\n\n";
    ct.prettyPrint(std::cout);
    */

}

