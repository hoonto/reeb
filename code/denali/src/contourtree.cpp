#include <map>
#include <vector>
#include <queue>
#include <list>
#include <iterator>
#include "scalargraph.h"
#include "contourtree.h"
#include "disjointsetforest.h"
#include "common.h"


void ContourTree::computeJoinTree(ScalarGraph& sg, std::vector<NodeID>& order_to_id,
        std::map<NodeID,NodeID>& id_to_order) {
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


void ContourTree::computeSplitTree(ScalarGraph& sg, std::vector<NodeID>& order_to_id,
        std::map<NodeID,NodeID>& id_to_order) {
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


void ContourTree::mergeTrees() {
    is_rooted = false;
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


void ContourTree::computeAugmentedContourTree(ScalarGraph& plex) {
    
    // get the sorted nodes
    std::vector<NodeID> order_to_id = plex.getSortedNodes();

    // make the reverse mapping from id to order
    std::map<NodeID,NodeID> id_to_order;
    for (NodeID order=0; order<order_to_id.size(); ++order) {
        id_to_order[order_to_id[order]] = order;
    }

    ScalarGraph join_tree, split_tree;
    computeJoinTree(plex, order_to_id, id_to_order);
    computeSplitTree(plex, order_to_id, id_to_order);
    mergeTrees();
}


void ContourTree::computeContourTree(ScalarGraph& plex) {

    // first compute the augmented contour tree
    computeAugmentedContourTree(plex);

    // now remove the regular vertices
    removeRegularVertices();
    
}


void ContourTree::removeRegularVertices() {
    std::vector<NodeID> nodes = contour_tree.getNodes();    
    std::list<NodeID> regular_nodes;

    for (std::vector<NodeID>::iterator it=nodes.begin(); it!=nodes.end(); ++it) {
        if (isRegularNode(*it)) {
            regular_nodes.push_back(*it);
        }
    }

    for (std::list<NodeID>::iterator it=regular_nodes.begin(); it!=regular_nodes.end(); ++it) {

        std::list<NodeID> neighbors = contour_tree.getNeighbors(*it);

        // get the two neighbor node ids
        std::list<NodeID>::iterator neighbor = neighbors.begin();
        NodeID n1 = (*neighbor);
        neighbor++;
        NodeID n2 = (*neighbor);

        // get their members
        std::set<NodeID> m1 = contour_tree.edgeMembers(n1, *it);
        std::set<NodeID> m2 = contour_tree.edgeMembers(n2, *it);

        // union the members
        std::set<NodeID> members;
        members.insert(m1.begin(), m1.end());
        members.insert(m2.begin(), m2.end());

        // connect the other two nodes
        contour_tree.addEdge(n1, n2);

        // delete the middle node
        contour_tree.removeNode(*it);

        // update the new edge's members
        contour_tree.edgeMembers(n1, n2) = members;

    }

}


bool ContourTree::isRegularNode(NodeID u) {
    // Checks to see if a node u in g is regular (does it have exactly one
    // neighbors that is less than it, and one that is greater?
    unsigned int n_lesser = 0;
    unsigned int n_greater = 0;
    double u_height = contour_tree.getValue(u);

    std::list<NodeID> neighbors = contour_tree.getNeighbors(u);
    for (std::list<NodeID>::iterator it=neighbors.begin(); it!=neighbors.end(); ++it) {
        NodeID v = (*it);
        double v_height = contour_tree.getValue(v);
        
        if (v_height > u_height) {
            n_greater++;
        } else {
            n_lesser++;
        }
    }

    return n_greater==1 && n_lesser==1;
}


std::ostream& ContourTree::prettyPrint(std::ostream& os) {
    return contour_tree.prettyPrint(os);
}


bool ContourTree::isRooted() {
    return is_rooted;
}


void ContourTree::chooseRoot(NodeID root) {

}
