import networkx as nx
import numpy as np
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

def induce_total_volume(tree, root=None):
    """
    Given a tree with edges having the 'volume' metadata, computes the
    'total_volume' edge and node property which is the sum of all edge volumes
    between successors. If the 'root' parameter is not supplied, it will be
    determined by performing a topological sort.
    """
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
        domain.append([center[0], center[1], tree.node[node]['height']])
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
        domain.extend([[x[0], x[1], left_height] for x in left_pts])
        domain.extend([[x[0], x[1], right_height] for x in right_pts])
        # recurse
        enclose_contour(domain, tree, left_child, left_rect)
        enclose_contour(domain, tree, right_child, right_rect)
