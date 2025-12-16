// graph.cpp (Implementation of Class Graph)

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>
#include "graph.h"

const Graph::NodeId Graph::invalid_node = -1;
const double Graph::infinite_weight = std::numeric_limits<double>::max();

// (Fix: Correct scope resolution for static member)
const Graph::CapValue Graph::infinite_capacity = std::numeric_limits<int>::max();


void Graph::add_nodes(NodeId num_new_nodes)
{
   _nodes.resize(num_nodes() + num_new_nodes);
}


//Constructor now adds indices of edges and virtual edges, along with a default flow of 0.
//(Fix: Parameter name matched to header: reverse_edge -> reverse_edge_index)
Graph::Neighbor::Neighbor(Graph::NodeId n, double w, int index, int capacity, int reverse_edge_index): _id(n), _edge_weight(w), _edge_index(index), _capacity(capacity), _flow(0), _reverse_edge_index(reverse_edge_index){}

Graph::Graph(NodeId num, DirType dtype): dirtype(dtype), _nodes(num), _num_edge(0) {}


//getter method for edge index
int Graph::Neighbor::edge_index() const {return _edge_index;}

//getter method for num edge
int Graph::num_edge() const {return _num_edge;}


//flow-related getters
Graph::CapValue Graph::Neighbor::capacity() const {return _capacity;}
Graph::CapValue Graph::Neighbor::flow() const {return _flow;}
int Graph::Neighbor::reverse_edge() const {return _reverse_edge_index;}

//flow-related setters 
void Graph::Neighbor::set_flow(CapValue f) {_flow = f;}

Graph::CapValue Graph::Neighbor::residual_capacity() const {return _capacity - _flow;}

//add_edge now builds Residualgraph,
//meaning this function will automatically create
//a virtual edge on top of the actual edge.
void Graph::add_edge(NodeId tail, NodeId head, double weight, int index)
{
   if (tail >= num_nodes() or tail < 0 or head >= num_nodes() or head < 0) {
       throw std::runtime_error("Edge cannot be added due to undefined endpoint.");
   }

   //tail := u, head := v, edge := (u,v), virtual edge := (v,u)
   //index of the actual edge (u,v) is saved at the last element of the tail's list of neighbors
   int tail_neighbor_index = _nodes[tail].adjacent_nodes().size();
   //index of the virtual edge (v,u) is saved at the last element of the head's list of neighbors
   int head_neighbor_index = _nodes[head].adjacent_nodes().size();

   
   //creating actual edge (u,v), parsing the location of the partner virtual edge (v,u)
   _nodes[tail].add_neighbor(head, weight, index, (CapValue)weight, head_neighbor_index);
   
   //creating the virtual edge (v,u), parsing the location of the partner actual edge (u,v)
   //default setting flow = 0, index = -1
   if (dirtype == Graph::directed) {
        _nodes[head].add_neighbor(tail, 0.0,  -1, 0, tail_neighbor_index);
   }
   _num_edge++;
}

void Graph::Node::add_neighbor(Graph::NodeId nodeid, double weight, int index, CapValue capacity, int reverse_edge_index)
{
   _neighbors.push_back(Graph::Neighbor(nodeid, weight, index, capacity, reverse_edge_index));
}


//mutable getter
std::vector<Graph::Neighbor> & Graph::Node::adjacent_nodes_mutable() {
   return _neighbors;
}

const std::vector<Graph::Neighbor> & Graph::Node::adjacent_nodes() const
{
   return _neighbors;
}

Graph::NodeId Graph::num_nodes() const
{
   return _nodes.size();
}

//mutable getter (Fix: matched name to header)
Graph::Node & Graph::get_node_mutable(NodeId node) {
   if (node < 0 or node >= static_cast<int>(_nodes.size())) {
        throw std::runtime_error("Invalid nodeid in Graph::get_node.");
   }
   return _nodes[node];
}

const Graph::Node & Graph::get_node(NodeId node) const
{
   if (node < 0 or node >= static_cast<int>(_nodes.size())) {
        throw std::runtime_error("Invalid nodeid in Graph::get_node.");
   }
   return _nodes[node];
}

Graph::NodeId Graph::Neighbor::id() const
{
   return _id;
}

double Graph::Neighbor::edge_weight() const
{
   return _edge_weight;
}

void Graph::print() const
{
   if (dirtype == Graph::directed) {
        std::cout << "Digraph ";
   } else {
        std::cout << "Undirected graph ";
   }
   std::cout << "with " << num_nodes() << " vertices, numbered 0,...,"
              << num_nodes() - 1 << ".\n";

   for (auto nodeid = 0; nodeid < num_nodes(); ++nodeid) {
        std::cout << "The following edges are ";
        if (dirtype == Graph::directed) {
            std::cout << "leaving";
        } else {
            std::cout << "incident to";
        }
        std::cout << " vertex " << nodeid << ":\n";
        for (auto neighbor: _nodes[nodeid].adjacent_nodes()) {
            std::cout << nodeid << " - " << neighbor.id()
                      << " weight = " << neighbor.edge_weight() << "\n";
        }
   }
}

Graph::Graph(char const * filename, DirType dtype): dirtype(dtype)
{
   std::ifstream file(filename);                             // open file
   if (not file) {
        throw std::runtime_error("Cannot open file.");
   }

   Graph::NodeId num = 0;
   std::string line;
   std::getline(file, line);                 // get first line of file
   std::stringstream ss(line);               // convert line to a stringstream
   ss >> num;                                // for which we can use >>
   if (not ss) {
        throw std::runtime_error("Invalid file format.");
   }
   add_nodes(num);

   //initialise current edge index
   int curr_edge_index = 0;

   while (std::getline(file, line)) {
        std::stringstream ss(line);
        Graph::NodeId head, tail;

        ss >> tail >> head;
        if (not ss) {
            throw std::runtime_error("Invalid file format.");
        }

        //the weight of an edge represents the capacity of that edge
        double weight = 1.0;
        ss >> weight;
        if (tail != head) {
            add_edge(tail, head, weight, curr_edge_index);
            curr_edge_index++;
        }
        else {
            throw std::runtime_error("Invalid file format: loops not allowed.");
        }
   }
}