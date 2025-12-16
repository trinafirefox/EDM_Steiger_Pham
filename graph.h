// graph.h (Declaration of Class Graph)
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>

class Graph {
public:
  using NodeId = int;  // vertices are numbered 0,...,num_nodes()-1
  using CapValue = int; // adding some semantics for integer flow value

  class Neighbor {
  public:
        //the construtor should now receive an intger-valued index, and the index of the virtual edge
        Neighbor(Graph::NodeId n, double w, int index, int capacity, int reverse_edge_index);
        double edge_weight() const;
        Graph::NodeId id() const;

        //Getter-method to get edge´s index
        int edge_index() const;

        //flow-related getters
        CapValue capacity() const; //getter for cap
        CapValue flow() const; 
        int reverse_edge() const; //getter for index of the virtual edge
        
        void set_flow(CapValue f); //setter for the flow of an edge, for updates
        CapValue residual_capacity() const; //compute u(e)-f(e)
  private:
        Graph::NodeId _id;
        double _edge_weight;
        
        //edge´s index value
        int _edge_index;

        //flow-related member variables
        CapValue _capacity; //u(e)
        CapValue _flow; //f(e)
        int _reverse_edge_index; //index of edges in the neighbors of node with _id value

    };

  class Node {
  public:
        //neighbor now has extra index, must be parametrized
        void add_neighbor(Graph::NodeId nodeid, double weight, int index, CapValue capacity, int reverse_edge_index);
        const std::vector<Neighbor> & adjacent_nodes() const;

        //we want to change neighbors in the G_f, so this method should be mutatble
        std::vector<Neighbor> & adjacent_nodes_mutable();
  private:
        std::vector<Neighbor> _neighbors;
    };

  enum DirType {directed, undirected};  // enum defines a type with possible values
  Graph(NodeId num_nodes, DirType dirtype);
  Graph(char const* filename, DirType dirtype);

  void add_nodes(NodeId num_new_nodes);

  //adds edge with index of default value -1 so that we know that an edge with index -1 is virtual edge and the class still
  //functions without edge indices

  void add_edge(NodeId tail, NodeId head, double weight = 1.0, int index = -1);

  NodeId num_nodes() const;
  const Node & get_node(NodeId) const;
  
  //mutable version (Fixed Typo: mutatable -> mutable)
  Node & get_node_mutable(NodeId);

  void print() const;

  const DirType dirtype;
  static const NodeId invalid_node;
  static const double infinite_weight;
  
  // (Fix: Declared static constant inside Graph class)
  static const CapValue infinite_capacity;

  //getter for num_edge
  int num_edge() const;

private:
  std::vector<Node> _nodes;

  //saves the number of edges
  int _num_edge;
};

#endif // GRAPH_H