import cffi
import numpy as np

def contour_tree(values, edges):
    """
    Given a list of the values corresponding to len(values) vertices and a
    list of edges in a simplicial complex, computes the contour tree, 
    returning a list of the edges in the tree.
    """
    # establish the interface with the shared object
    ffi = cffi.FFI()
    ffi.cdef("void computeContourTree(size_t, double *, \
        size_t, unsigned long int **);")
    lib = ffi.dlopen("./pydenali.so")

    edge_array = []
    for x in edges:
        a = ffi.new("unsigned long int[]", x)
        edge_array.append(a)

    ct = lib.computeContourTree(len(values), values, len(edges), edge_array)

