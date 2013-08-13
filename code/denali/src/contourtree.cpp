#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <queue>
#include "contourtree.h"
#include "scalargraph.h"
#include "disjointsetforest.h"

void ContourTree::compute(ScalarGraph& sg) {
    // get the sorted nodes
    std::vector<int> order_to_node = sg.getSortedNodes();

    // establish a mapping from the node id to position in the enumeration
    std::map<int,int> node_to_order;
    for (int i=0; i<order_to_node.size(); ++i) {
        node_to_order[order_to_node[i]] = i;
    }

    ScalarGraph join_tree = joinTree(sg, order_to_node, node_to_order);
    ScalarGraph split_tree = splitTree(sg, order_to_node, node_to_order);

    mergeTree(join_tree, split_tree);

}

void ContourTree::addEdge(int x, int y) {
    // add edge from x to y
    nodes[x].neighbors.insert(y);
}

ScalarGraph ContourTree::joinTree(ScalarGraph& sg, std::vector<int>& order_to_node, std::map<int,int>& node_to_order) {
    ContourTree tree;
    DisjointSetForest forest;

    // iterate through all of the vertices from least to greatest
    for (std::vector<int>::iterator it_i=order_to_node.begin(); it_i!=order_to_node.end(); ++it_i) {
        int vi = *it_i;
        tree.addNode(vi, sg.getValue(vi));
        forest.makeSet(vi);

        // iterate through the neighbors
        std::set<int> nbrs = sg.neighbors(vi);
        for (std::set<int>::iterator it_j=nbrs.begin(); it_j!=nbrs.end(); ++it_j) {
            int vj = *it_j;
            int i = node_to_order[vi];
            int j = node_to_order[vj];
            int set_i = forest.findSet(vi);
            int set_j = forest.findSet(vj);

            if (j<i && set_i!=set_j) {
                int k = 0;
                int vk = 0;
                vk = forest.maxSet(vj);
                tree.addEdge(vi,vk);
                forest.unionSets(vi,vj,node_to_order);
            }
        }
    }

    return tree;
}

ScalarGraph ContourTree::splitTree(ScalarGraph& sg, std::vector<int>& order_to_node, std::map<int,int>& node_to_order) {
    ContourTree tree;
    DisjointSetForest forest;

    // iterate through all of the vertices from least to greatest
    for (std::vector<int>::reverse_iterator it_i=order_to_node.rbegin(); it_i!=order_to_node.rend(); ++it_i) {
        int vi = *it_i;
        tree.addNode(vi, sg.getValue(vi));
        forest.makeSet(vi);

        // iterate through the neighbors
        std::set<int> nbrs = sg.neighbors(vi);
        for (std::set<int>::iterator it_j=nbrs.begin(); it_j!=nbrs.end(); ++it_j) {
            int vj = *it_j;
            int i = node_to_order[vi];
            int j = node_to_order[vj];
            int set_i = forest.findSet(vi);
            int set_j = forest.findSet(vj);

            if (j>i && set_i!=set_j) {
                int k = 0;
                int vk = 0;
                vk = forest.minSet(vj);
                tree.addEdge(vi,vk);
                forest.unionSets(vi,vj,node_to_order);
            }
        }
    }

    return tree;
}

void ContourTree::mergeTree(ScalarGraph& jt, ScalarGraph& st) {
    // clear self
    this->clear();

    std::queue<int> q;
    
    // add every node to the contour tree
    std::vector<int> nodes = jt.getNodes();
    for (std::vector<int>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
        int vi = *it;
        addNode(vi, jt.getValue(vi));
        if (jt.neighbors(vi).size() + st.neighbors(vi).size() == 1) {
            q.push(vi);
        }
    }

    while (q.size() > 1) {
        int vi = q.front();
        q.pop();

        if (jt.neighbors(vi).size() == 0) {
            int vk =  
        }
    }
}
