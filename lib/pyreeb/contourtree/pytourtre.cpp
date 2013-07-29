#include <iostream>
#include <fstream>
#include <vector>
extern "C" 
{
#include <tourtre.h>
}

using std::cout;

struct vertex {
	double value;
	std::vector<int> neighbors;
};

double get_value ( size_t v, void * d ) {
	// reinterpret the pointer as one to a vector of vertices
	std::vector<vertex> * vertices = reinterpret_cast<std::vector<vertex>*>(d);
	// fill the nbrs array with the neighbors of this vertex
	vertex x = (*vertices).at(v);
	return x.value;
}


size_t get_neighbors ( size_t v, size_t * nbrs, void * d ) {
	// reinterpret the pointer as one to a vector of vertices
	std::vector<vertex> * vertices = reinterpret_cast<std::vector<vertex>*>(d);
	// fill the nbrs array with the neighbors of this vertex
	vertex x = (*vertices).at(v);
	for (int i=0; i<x.neighbors.size(); i++){
		nbrs[i] = (size_t) x.neighbors[i];
	}
	return (size_t) x.neighbors.size();
}

void print_vertices(std::vector<vertex> & vertices){

	int num_vertices = vertices.size();

	for (int i=0; i<num_vertices; i++){
		cout << "Node " << i << "\n";
		cout << "\tNeighbors: ";
		for (int j=0; j<vertices[i].neighbors.size(); j++){
			cout << vertices[i].neighbors[j] << " ";
		}
		cout << "\n";
		cout << "\tValue: " << vertices[i].value << "\n";
		cout << "\n";
	}
}


extern "C" void compute_contour_tree(
		size_t num_vertices,
		int ** neighbors,
		int num_neighbors[],
		double * values,
		int * num_out_nodes,
		int ** out_nodes,
		int * num_out_edges,
		int *** out_edges
	){

	// make a vector of the vertices
	std::vector<vertex> vertices;	
	vertices.clear();

	// add each vertex to the vector of vertices, one at a time
	for (int i=0; i<num_vertices; i++){
		vertex v;
		v.value = values[i];

		// add each of this node's neighbors
		for (int j=0; j<num_neighbors[i]; j++){
			v.neighbors.push_back(neighbors[i][j]);
		}

		vertices.push_back(v);
	}

	// print the vertices in order to debug
	print_vertices(vertices);


	std::vector<size_t> total_order;
	for (int i=0; i<num_vertices; i++){
		total_order.push_back(i);
	}

	ctContext * ctx = ct_init(
		num_vertices, //numVertices
		&(total_order.front()), //totalOrder. Take the address of the front of an stl vector, which is the same as a C array
		&get_value,
		&get_neighbors,
		&vertices //data for callbacks. The global functions less, value and neighbors are just wrappers which call mesh->getNeighbors, etc
	);
	
	//create contour tree
	ctArc * a = ct_sweepAndMerge( ctx );

	// get lists of all of the nodes and arcs
	ctArc *** arcs = (ctArc ***) malloc(1);
	ctNode *** nodes = (ctNode ***) malloc(100);
	size_t * numarcs = new size_t;
	size_t * numnodes = new size_t;
	ct_arcsAndNodes(
		a,
		arcs,
		numarcs,
		nodes,
		numnodes
	);

	// return the number of out nodes and edges
	*num_out_nodes = *numnodes;
	*num_out_edges = *numarcs;

	// return the mapping from nodes onto arcs
	ctArc ** arc_mapping = ct_arcMap( ctx );
	int * num_nodes_in_arc;
	num_nodes_in_arc = new int [*numarcs];

	for (int i=0; i<*numarcs; i++){
		num_nodes_in_arc[i] = 0;
	}

	for (int i=0; i<num_vertices; i++){
		for (int j=0; j<*numarcs; j++){
			if (arc_mapping[i] == (*arcs)[j]){
				num_nodes_in_arc[j]++;
			}
		}
	}

	// allocate the array holding the indices of the nodes
	(*out_nodes) = new int [*numnodes];
	for (int i=0; i<*numnodes; i++){
		ctNode * n = (*nodes)[i];
		(*out_nodes)[i] = n->i;
	}

	// allocate the array holding the edges
	(*out_edges) = new int * [*numarcs];
	for (int i=0; i<*numarcs; i++){
		(*out_edges)[i] = new int [3];
		ctArc * arc = (*arcs)[i];
		ctNode * hi = arc->hi;
		ctNode * lo = arc->lo;
		(*out_edges)[i][0] = hi->i;
		(*out_edges)[i][1] = lo->i;
		(*out_edges)[i][2] = num_nodes_in_arc[i];
	}

	ct_cleanup( ctx );
}
