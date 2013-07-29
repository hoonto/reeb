import networkx as nx
import numpy as np
import cffi
import re
import os
import os.path

def induce_tree(graph, node):
    """
    Given an undirected, acyclic graph, returns the tree induced by taking
    'node' as the root node. Preserves edge and node metadata.
    """
    tree = nx.dfs_tree(graph, node)
    # preserve node metadata
    for u in tree.nodes_iter():
        tree.node[u] = graph.node[u]
    # preserve edge metadata
    for s in tree.edges_iter():
        tree.edge[s[0]][s[1]] = graph.edge[s[0]][s[1]]

    return tree


def induce_total_volume(volumeless_tree, root=None):
    """
    Given a tree with edges having the 'volume' metadata, computes the
    'total_volume' edge and node property which is the sum of all edge volumes
    between successors. If the 'root' parameter is not supplied, it will be
    determined by performing a topological sort.
    """
    tree = volumeless_tree.copy()
    if root is None:
        root = nx.topological_sort(tree)[0]

    # compute the depth-first search traversal, returning nodes in post-order
    traversal = nx.dfs_postorder_nodes(tree, root)
    for u in traversal:
        tree.node[u]['total_volume'] = 0
        for v in tree.successors(u):
            tree.node[u]['total_volume'] += (tree.node[v]['total_volume'] +
                tree.edge[u][v]['volume'])
            tree.edge[u][v]['total_volume'] = (tree.edge[u][v]['volume'] + 
                tree.node[v]['total_volume'])

    return tree


def contract_augmented_reeb_graph(g):
    """
    Given an augmented reeb graph, contracts the graph so that all remaining
    nodes are of either degree 1 (leaf) or degree 3 (saddle point).
    """
    h = g.copy()

    # ensure that each edge in h has the component property
    for e in h.edges_iter():
        h[e[0]][e[1]]['component'] = set()
    link_nodes = [v for v in h if len(h.neighbors(v))==2]

    for v in link_nodes:
        # link the two neighbors together
        neighbors = h.neighbors(v)
        # keep track of the edge's components
        component = set([v])
        component = component | h.edge[neighbors[0]][v]['component']
        component = component | h.edge[neighbors[1]][v]['component']
        h.add_edge(neighbors[0], neighbors[1], component=component)
        # remove the linking node from the graph
        h.remove_node(v)

    return h


def endow_graph(g, parameter_space, likelihoods):
    """
    Given a contracted reeb graph with the component edge property, endows
    the graph nodes with the 'height' property and the edges with the
    'volume' property.
    """
    h = g.copy()
    for v in h:
        h.node[v]['height'] = likelihoods[v][-1]
    for e in h.edges_iter():
        h[e[0]][e[1]]['volume'] = len(h[e[0]][e[1]]['component']) + 1
    return h


def compute(neighbors, values):
    """
    Given the neighbors of each node, and the values at each node, returns
    the contour tree as a networkx object.

    Interfaces with the library using CFFI.
    """
    ffi = cffi.FFI()

    # define the library functions
    ffi.cdef("void compute_contour_tree( \
            size_t num_vertices, \
            int ** neighbors, \
            int num_neighbors[], \
            double * values, \
            int * num_out_nodes, \
            int ** out_nodes, \
            int * num_out_edges, \
            int *** out_edges \
            );")

    # load the library
    libpath = os.path.join(os.path.dirname(os.path.realpath(__file__)), 
        "pytourtre.so")
    lib = ffi.dlopen(libpath)

    # create a neighbors array
    list_of_neighbor_arrays = []
    number_of_neighbors = []
    for x in neighbors:
        list_of_neighbor_arrays.append(ffi.new("int []", x))
        number_of_neighbors.append(len(x))
    arr_neighbors = ffi.new("int * []", list_of_neighbor_arrays)

    # define the output variables as C types
    num_out_nodes = ffi.new("int *")
    num_out_edges = ffi.new("int *")
    out_nodes = ffi.new("int **")
    out_edges = ffi.new("int ***")

    lib.compute_contour_tree(
        len(neighbors),
        arr_neighbors,
        number_of_neighbors,
        values,
        num_out_nodes,
        out_nodes,
        num_out_edges,
        out_edges)

    tree_nodes = [out_nodes[0][i] for i in range(num_out_nodes[0])]
    tree_edges = [(out_edges[0][i][0], out_edges[0][i][1], out_edges[0][i][2])\
        for i in range(num_out_edges[0])]

    return tree_nodes, tree_edges
