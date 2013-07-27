import networkx as nx
import numpy as np
import re
import pdb

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

def scale_rectangle(rect, final_area):
    """
    Scales a rectange, given in the form of the 4-tuple 
    (xmin, xmax, ymin, ymax), so that the area of the result is 'final_area'.
    The center of the rectangle is not changed. Returns a 4-tuple.
    """
    xmin, xmax, ymin, ymax = rect
    aspect_ratio = (ymax - ymin)/float(xmax - xmin)
    new_width = np.sqrt(final_area/aspect_ratio)
    new_height = aspect_ratio*new_width
    x_delta = (new_width - (xmax - xmin))/2
    y_delta = (new_width - (ymax - ymin))/2
    return (xmin-x_delta, xmax+x_delta, ymin-y_delta, ymax+y_delta)

def rectangle_bounds_to_points(rect):
    """
    Given a rectangle in the form (xmin, xmax, ymin, ymax), returns a list
    of the four corner points.
    """
    xmin, xmax, ymin, ymax = rect
    points = []
    points.append([xmin,ymin])
    points.append([xmin,ymax])
    points.append([xmax,ymin])
    points.append([xmax,ymax])
    return points

def enclose_contour(domain, tree, node, rect):
    """
    Recursively places landscape countour rectangles withing the bounding
    rectangle 'rect'.
    """
    if len(tree.successors(node)) == 0:
        center = (rect[0]+rect[1])/2., (rect[2]+rect[3])/2. 
        domain.append([center[0], center[1], tree.node[node]['height'], node])
    else:
        # this is a degree 3 node, so there will be an inner bounding box
        # here, we calculate the volume of the bounding rectangle
        inner_volume = float(tree.node[node]['total_volume'])
        # scale down the outer rectangle to obtain the inner rectangle
        inner_rect = scale_rectangle(rect, inner_volume)
        xmin, xmax, ymin, ymax = inner_rect
        # now we calculate the position of the dividing line in the rectangle,
        # which separates the left and right children
        left_child, right_child = tree.successors(node)
        left_ratio = tree.edge[node][left_child]['total_volume']/inner_volume
        x_divider = xmin + left_ratio*(xmax-xmin)
        left_rect = xmin, x_divider, ymin, ymax
        right_rect = x_divider, xmax, ymin, ymax
        # add the rectangle points to the domain
        left_pts = rectangle_bounds_to_points(left_rect)
        right_pts = rectangle_bounds_to_points(right_rect)
        left_height = tree.node[left_child]['height']
        right_height = tree.node[right_child]['height']
        domain.extend([[x[0], x[1], left_height, left_child] for x in left_pts])
        domain.extend([[x[0], x[1], right_height, right_child] for x in right_pts])
        # recurse
        enclose_contour(domain, tree, left_child, left_rect)
        enclose_contour(domain, tree, right_child, right_rect)

def read_OFF(filename):
    """
    Given the path of an OFF file, reads in the file, returning a list of the
    vertex positions and a list of each simplex in the file. 

    This is useful to read the result of RandReeb. Since RandReeb takes as
    input an OFF file where the vertices are embedded in either R^2 or R^3,
    and we are generally dealing with high-dimensional data, the vertex list
    is generally going to be useless. The list of simplices, however, will
    be an edge list. We can then represent the graph easily using networkx:
        
        vertices, simplices = read_OFF(filename)
        g = networkx.Graph()
        g.add_edges_from(simplices)

    """
    with open(filename) as f:
        # read until we hit the line describing the number of vertices
        while True:
            line = f.readline().strip()
            match = re.match("^(\d+) (\d+) (\d+)$", line)
            if match:
                n_vertices = int(match.groups()[0])
                n_simplices = int(match.groups()[1])
                break
        # read in the vertices
        vertices = []
        for i in range(n_vertices):
            v = [float(x) for x in f.readline().strip().split()]
            vertices.append(v)
        # read in the simplices
        simplices = []
        for i in range(n_simplices):
            line = f.readline().strip().split()
            dim = int(line[0])
            s = [int(x) for x in line[1:dim+1]]
            simplices.append(s)
    return vertices, simplices

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
