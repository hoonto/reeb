#include <map>
#include <vector>
#include <queue>
#include "scalargraph.h"
#include "contourtree.h"
#include "disjointsetforest.h"
#include "common.h"


namespace {
void computeJoinTree(ScalarGraph& sg, ScalarGraph& join_tree, 
        std::vector<NodeID>& order_to_id, std::map<NodeID,NodeID>& id_to_order) {
    join_tree.clear();
    DisjointSetForest forest;

    // iterate through the vertices in ascending order
    for (std::vector<NodeID>::iterator it_i=order_to_id.begin(); it_i!=order_to_id.end(); ++it_i) {
        NodeID vi = *it_i;
        join_tree.addNode(vi, sg.getValue(vi));
        forest.makeSet(vi);
        
        // iterate through each of the neighbors
        std::list<NodeID> neighbors = sg.getNeighbors(vi);
        for (std::list<NodeID>::iterator it_j=neighbors.begin(); it_j!=neighbors.end(); ++it_j) {
            // obtain the node's ID
            NodeID vj = *it_j;
            // get the position of the nodes in the ordering
            NodeID i = id_to_order[vi]; 
            NodeID j = id_to_order[vj];
            // get the representative ids of each set
            NodeID rep_i = forest.findSet(vi); 
            NodeID rep_j = forest.findSet(vj);
            
            if (j<i && rep_i!=rep_j) {
                // get the max member of the set containing vj
                NodeID vk = forest.maxSet(vj);
                // make vk the parent of vj in the join tree
                join_tree.addEdge(vi,vk);
                // union the sets
                forest.unionSets(vi,vj,id_to_order);
            }
            
        }
    }
}
}


namespace {
void computeSplitTree(ScalarGraph& sg, ScalarGraph& split_tree,
        std::vector<NodeID>& order_to_id, std::map<NodeID,NodeID>& id_to_order) {
    split_tree.clear();
    DisjointSetForest forest;

    // iterate through the vertices in reverse order
    for (std::vector<NodeID>::reverse_iterator it_i=order_to_id.rbegin(); it_i!=order_to_id.rend(); ++it_i) {
        NodeID vi = *it_i;
        split_tree.addNode(vi, sg.getValue(vi));
        forest.makeSet(vi);

        // iterate through each of the neighbors
        std::list<NodeID> neighbors = sg.getNeighbors(vi);
        for (std::list<NodeID>::iterator it_j=neighbors.begin(); it_j!=neighbors.end(); ++it_j) {
            // obtain the node's ID
            NodeID vj = *it_j;
            // get the position of the nodes in the ordering
            NodeID i = id_to_order[vi]; 
            NodeID j = id_to_order[vj];
            // get the representative ids of each set
            NodeID rep_i = forest.findSet(vi); 
            NodeID rep_j = forest.findSet(vj);

            if (j>i && rep_i!=rep_j) {
                // get the max member of the set containing vj
                NodeID vk = forest.minSet(vj);
                // make vk the parent of vj in the join tree
                split_tree.addEdge(vi,vk);
                // union the sets
                forest.unionSets(vi,vj,id_to_order);
            }
        }
    }
}


void mergeTrees(ScalarGraph& join_tree, ScalarGraph& split_tree, ScalarGraph& contour_tree) {
    contour_tree.clear();
    std::vector<NodeID> nodes = join_tree.getNodes();  
    std::queue<NodeID> merge_queue;

    // initialize the tree with vertices and add to the merge queue
    for (std::vector<NodeID>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
        NodeID ai = *it;
        // create the vertex in the contour tree
        contour_tree.addNode(ai, join_tree.getValue(ai));
        if (join_tree.getNumberOfChildren(ai) + split_tree.getNumberOfChildren(ai) == 1)
            merge_queue.push(ai);
    }

    while (merge_queue.size()>1) {
        // get the value at the front of the queue
        NodeID& vi = merge_queue.front();
        
        if (join_tree.getNumberOfChildren(vi) == 0) {
            // get the predecessor of this node in the join tree
            NodeID vk = join_tree.getPredecessors(vi).front();
            // make vk the parent of vi in the contour tree
            contour_tree.addEdge(vk,vi);
            // delete the node from the join tree
            join_tree.removeNode(vi);
            // connect vi's parent to vi's child in the split tree
            if (split_tree.getNumberOfPredecessors(vi) > 0) {
                NodeID split_predecessor = split_tree.getPredecessors(vi).front();
                NodeID split_child = split_tree.getChildren(vi).front();
                split_tree.addEdge(split_predecessor, split_child);
            }
            split_tree.removeNode(vi);
            // add to the queue if necessary
            if (join_tree.getNumberOfChildren(vk) + split_tree.getNumberOfChildren(vk) == 1) {
                merge_queue.push(vk);
            }
        } else {
            // get the predecessor of this node in the join tree
            NodeID vk = split_tree.getPredecessors(vi).front();
            // make vk the parent of vi in the contour tree
            contour_tree.addEdge(vk,vi);
            // delete the node from the join tree
            split_tree.removeNode(vi);
            // connect vi's parent to vi's child in the split tree
            if (join_tree.getNumberOfPredecessors(vi) > 0) {
                NodeID join_predecessor = join_tree.getPredecessors(vi).front();
                NodeID join_child = join_tree.getChildren(vi).front();
                join_tree.addEdge(join_predecessor, join_child);
            }
            join_tree.removeNode(vi);
            // add to the queue if necessary
            if (join_tree.getNumberOfChildren(vk) + split_tree.getNumberOfChildren(vk) == 1) {
                merge_queue.push(vk);
            }
        }
        merge_queue.pop();
    }
}
}


void denali::computeAugmentedContourTree(ScalarGraph& plex, ScalarGraph& contour_tree) {
    
    // get the sorted nodes
    std::vector<NodeID> order_to_id = plex.getSortedNodes();

    // make the reverse mapping from id to order
    std::map<NodeID,NodeID> id_to_order;
    for (NodeID order=0; order<order_to_id.size(); ++order) {
        id_to_order[order_to_id[order]] = order;
    }

    ScalarGraph join_tree, split_tree;
    computeJoinTree(plex, join_tree, order_to_id, id_to_order);
    computeSplitTree(plex, split_tree, order_to_id, id_to_order);
    mergeTrees(join_tree, split_tree, contour_tree);
}


void denali::computeContourTree(ScalarGraph& plex, ScalarGraph& contour_tree) {

    // first compute the augmented contour tree
    denali::computeAugmentedContourTree(plex, contour_tree);

    // now remove the regular vertices
    denali::removeRegularVertices(contour_tree);
    
}


void denali::removeRegularVertices(ScalarGraph& sg) {
    
}
