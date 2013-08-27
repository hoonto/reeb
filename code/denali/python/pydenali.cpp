#include <stdio.h>
#include <iostream>
#include <vector>
#include "scalargraph.h"
#include "contourtree.h"

extern "C" void computeContourTree(
        size_t n_nodes, double * values, size_t n_edges, unsigned long int ** edges) {

    ScalarGraph plex;
    // add all of the nodes first
    for (int i=0; i<n_nodes; ++i) {
        plex.addNode(i, values[i]);
    }

    // now add all of the edges
    for (int i=0; i<n_edges; ++i) {
        plex.addEdge(edges[i][0], edges[i][1]);
    }

    ScalarGraph ct;
    computeContourTree(plex, ct);

}
