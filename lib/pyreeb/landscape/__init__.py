import numpy as np
import networkx as nx

class Rectangle():
    def __init__(self, x_min=0, x_max=1, y_min=0, y_max=1):
        self._bounds = [x_min, x_max, y_min, y_max]
        
    def __repr__(self):
        return "Rectangle(x_min={0}, x_max={1}, y_min={2}, y_max={3})".format(*self._bounds)
        
    def points(self):
        """
        Returns the corner points of the rectangle as a list.
        The list's order is set such that the 0th point is the lower left, and the
        indices increase clockwise.
        """
        xmin, xmax, ymin, ymax = self._bounds
        points = []
        points.append([xmin,ymin])
        points.append([xmin,ymax])
        points.append([xmax,ymax])
        points.append([xmax,ymin])
        return points
    
    def scale(self, final_area):
        """
        Scales the rectangle to area final_area without translation.
        """
        xmin, xmax, ymin, ymax = self._bounds
        aspect_ratio = self.aspect_ratio()
        new_width = np.sqrt(final_area/aspect_ratio)
        new_height = aspect_ratio*new_width
        x_delta = (new_width - (xmax - xmin))/2
        y_delta = (new_width - (ymax - ymin))/2
        self._bounds = [xmin-x_delta, xmax+x_delta, ymin-y_delta, ymax+y_delta]
        
    def copy(self):
        return Rectangle(*self._bounds)
    
    def bounds(self):
        return list(self._bounds)
        
    def width(self):
        return self._bounds[1] - self._bounds[0]
    
    def height(self):
        return self._bounds[3] - self._bounds[2]
    
    def area(self):
        return self.height()*self.width()
    
    def center(self):
        xmin, xmax, ymin, ymax = self._bounds
        return (xmax+xmin)/2.0, (ymax+ymin)/2.0
    
    def aspect_ratio(self):
        return self.height()/float(self.width())
    
    def split(self, pct):
        """
        Splits the rectangle in two, such that the left rectangle has pct% of the total area.
        """
        xmin, xmax, ymin, ymax = self._bounds
        split_point = self.width()*float(pct) + xmin
        left_rectangle = Rectangle(xmin, split_point, ymin, ymax)
        right_rectangle = Rectangle(split_point, xmax, ymin, ymax)
        return left_rectangle, right_rectangle

def embed_contour_tree(tree, root_node, bounding_rectangle=None):
    """
    Given a tree and a root node, draws the landscape metaphor. The landscape
    is contained within the domain specified by the bounding_rectangle.
    """
    landscape_tree = tree.copy()
    if bounding_rectangle is None:
        bounding_rectangle = Rectangle(x_min=0,x_max=32,y_min=0,y_max=32)

    # embed the root contour
    landscape_tree.node[root_node]['rectangle'] = bounding_rectangle

    # there should be a single child
    successors = tree.successors(root_node)
    assert len(successors)==1, "Root node is not degree 1."

    # the successor should be degree 3
    assert len(tree.successors(successors[0]))==2, "Root node does not point to node of degree 3."
    embed_contour_subtree(landscape_tree, successors[0], bounding_rectangle)
    return landscape_tree      


def embed_contour_subtree(tree, node, parent_rectangle):
    """
    Recursively embeds the subtree rooted at node in bounding rectangle.
    """
    # we must check to see if the node is degree 1 (a leaf) or degree 3.
    is_leaf = len(tree.successors(node))==0
    if is_leaf:
        embed_d1_child(tree, node, parent_rectangle)
    else:
        left_rectangle, right_rectangle = embed_d3_child(tree, node, parent_rectangle)
        # now recurse for the two child nodes
        left_child, right_child = tree.successors(node)
        embed_contour_subtree(tree, left_child, left_rectangle)
        embed_contour_subtree(tree, right_child, right_rectangle)


def embed_d1_child(tree, node, parent_rectangle):
    """
    Embeds a leaf node, returning a leaf dictionary.
    """
    position = parent_rectangle.center()
    tree.node[node]['position'] = position
    return position


def embed_d3_child(tree, node, parent_rectangle):
    """
    Embeds a degree 3 node, returning a left contour and a right contour.
    """
    parent = tree.predecessors(node)[0]

    # scale the parent contour down
    parent_area = parent_rectangle.area()
    parent_volume = tree.node[parent]['total_volume']
    child_volume = tree.node[node]['total_volume']
    child_area = float(child_volume)/parent_volume * parent_area
    child_rectangle = parent_rectangle.copy()
    child_rectangle.scale(child_area)

    # divide the child rectangle in two according to the left and right branch volumes
    left_child, right_child = tree.successors(node)
    left_volume = tree.edge[node][left_child]['total_volume']
    right_volume = tree.edge[node][right_child]['total_volume']
    left_share = left_volume/float(child_volume)

    # split the child rectangle
    left_rectangle, right_rectangle = child_rectangle.split(left_share)
    tree.node[node]['left_rectangle'] = left_rectangle
    tree.node[node]['right_rectangle'] = right_rectangle

    return left_rectangle, right_rectangle       


def triangulate_landscape(landscape, root_node, normalize=True, scene_size=None):
    """
    Produce a triangular mesh of the landscape.
    """
    tris = np.empty((0,3))
    points = np.empty((0,3))

    # add the points from the root node's contour
    root_props = landscape.node[root_node]
    height_vec = np.repeat(root_props['height'],4)
    root_points = np.column_stack((root_props['rectangle'].points(), height_vec))
    points = np.vstack((points, root_points))

    # now recursively add the points from the subtree
    child = landscape.successors(root_node)[0]
    bounding_indices = [0,1,2,3]
    points, tris = triangulate_sublandscape(landscape, child, points, tris, bounding_indices)

    if normalize:
        if scene_size is None:
            scene_size = landscape.node[root_node]['rectangle'].width()
        height_range = np.ptp(points[:,2])
        scale_factor = scene_size/height_range
        points[:,2] = scale_factor*points[:,2]

    return points, tris


def triangulate_sublandscape(landscape, node, points, tris, bounding_indices):
    """
    Recursively triangulates a mesh of the landscape. The input node is either
    degree 3 or degree 1 (a leaf).
    """
    is_leaf = len(landscape.successors(node))==0
    if is_leaf:
        points,tris = triangulate_d1_child(landscape, node, points, tris, bounding_indices)
    else:
        points,tris,left_indices,right_indices = triangulate_d3_child(landscape, node, points, tris, bounding_indices)
        # recursively add points from the left and right branches
        left_child,right_child = landscape.successors(node)
        points,tris = triangulate_sublandscape(landscape, left_child, points, tris, left_indices)
        points,tris = triangulate_sublandscape(landscape, right_child, points, tris, right_indices)

    return points, tris


def triangulate_d1_child(landscape, node, points, tris, bounding_indices):
    """
    Triangulate a leaf node.
    """
    # add the leaf's points to the big points list
    leaf_position = landscape.node[node]['position']
    leaf_height = landscape.node[node]['height']
    leaf_point = np.append(leaf_position, leaf_height)
    points = np.vstack((points, leaf_point))

    # now hook up the bounding indices to the point
    pleaf = points.shape[0] - 1
    p0,p1,p2,p3 = bounding_indices
    leaf_tris = [[p0,p1,pleaf], [p1,p2,pleaf], [p2,p3,pleaf], [p3,p0,pleaf]]
    tris = np.vstack((tris, leaf_tris))

    return points, tris


def triangulate_nested_rectangles(inner, outer):
    """
    Given the indices of the corners of rectangles in clockwise order, generates a list of
    the triangles in the triangular mesh.
    """
    o0,o1,o2,o3 = outer
    i0,i1,i2,i3 = inner
    tris = [[o0,o1,i0], [i0,i1,o1], [o1,o2,i1], [i1,i2,o2], [i2,o2,o3], [i2,i3,o3],
        [i3,o3,o0], [i0,o0,i3]]
    return tris


def triangulate_d3_child(landscape, node, points, tris, bounding_indices):
    # add the d3 node's points to the point list. We will add the points in
    # clockwise order so that the corner points of the bounding rectangle are
    # the 0,1,3,4 points in the ordering
    left_points = landscape.node[node]['left_rectangle'].points()
    right_points = landscape.node[node]['right_rectangle'].points()
    node_points = []
    node_points.extend(left_points[0:3])
    node_points.extend(right_points[2:4])
    node_points.append(left_points[-1])

    # add the height of the node to the points
    height_vec = np.repeat(landscape.node[node]['height'],6)
    node_points = np.column_stack((node_points, height_vec))
    points = np.vstack((points, node_points))

    # calculate the indices of the node points
    base_index = points.shape[0]-6
    node_indices = base_index + np.array([0,1,3,4])
    left_indices = base_index + np.array([0,1,2,5])
    right_indices = base_index + np.array([5,2,3,4])
    node_tris = triangulate_nested_rectangles(node_indices, bounding_indices)
    tris = np.vstack((tris, node_tris))

    return points, tris, left_indices, right_indices


def rebase_landscape(landscape, new_root, side=None):
    """
    "Rebase" a landscape by choosing a new root. Essentially zooms into the landscape metaphor.
    """
    if side is None:
        left_rectangle = landscape.node[new_root]['left_rectangle']
        right_rectangle = landscape.node[new_root]['right_rectangle']
        side = 'left' if left_rectangle.area() > right_rectangle.area() else 'right'

    # copy the old landscape
    new_landscape = landscape.copy()

    # delete all nodes not in the subtree
    subtree = nx.dfs_tree(landscape, new_root)
    new_landscape.remove_nodes_from(set(new_landscape) - set(subtree))

    # now we need to choose the left or right subtree
    if side == 'left':
        new_landscape.node[new_root]['rectangle'] = landscape.node[new_root]['left_rectangle']
        new_landscape.remove_node(new_landscape.successors(new_root)[1])
    elif side == 'right':
        new_landscape.node[new_root]['rectangle'] = landscape.node[new_root]['right_rectangle']
        new_landscape.remove_node(new_landscape.successors(new_root)[0])
    else:
        raise Exception("Side not left or right.")

    return new_landscape
