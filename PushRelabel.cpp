#include <iostream>
#include <vector>
#include <algorithm>
#include "graph.h"

using CapValue = Graph::CapValue;

//global variables
std::vector<CapValue> excess;
std::vector<int> height;

//for Highest-Label approach
std::vector<std::vector<int>> buckets;

//all heights except phi(s) are null
int max_height = 0;

CapValue get_residual_capacity(Graph &G, int u, Graph::Neighbor &edge) {
    if (edge.edge_index() != -1) {
        //Actual edges
        //compute uf(e) = u(e) - f(e)
        return edge.capacity() - edge.flow();
    } else {
        //Virtual edges
        //uf(backwards e) = f(e)
        //we simply retrieve the partner's flow
        Graph::Node &target_node = G.get_node_mutable(edge.id());
        Graph::Neighbor &partner = target_node.adjacent_nodes_mutable()[edge.reverse_edge()];
        return partner.flow();
    }
}

void update_flow(Graph &G, int u, Graph::Neighbor &edge, int gamma) {
    if (edge.edge_index() != -1) {
        //actual edges:
        edge.set_flow(edge.flow() + gamma);
    } else {
        Graph::Node &target_node = G.get_node_mutable(edge.id());
        Graph::Neighbor &partner = target_node.adjacent_nodes_mutable()[edge.reverse_edge()];
        partner.set_flow(partner.flow() - gamma);
    }
}

//add nodes to buckets
void activate_node(int v, int n, int t) {
    //activation condition:
    //the node is not sink or source
    //excess must be strictly positive
    //height of s is n, height of t is 0.
    // Assuming s=0, t=1 based on main logic
    if (v != t && v != 0 && excess[v] > 0) {
        buckets[height[v]].push_back(v);
        if (height[v] > max_height) {
            max_height = height[v];
        }
    }
}

//Push
void push(Graph &G, int u, Graph::Neighbor &edge) {
    int v = edge.id();
    CapValue res_cap = get_residual_capacity(G,u,edge);
    
    //compare
    CapValue gamma = std::min(res_cap, excess[u]);

    //if nothing has changed
    if (gamma == 0) return;

    //increase flow on the actual edge
    update_flow(G, u, edge, gamma);

    excess[u] -= gamma;
    excess[v] += gamma;

    activate_node(v, G.num_nodes(), 1);
}

void relabel(Graph &G, int u) {
    int min_height = 2 * G.num_nodes();
    //find min along heights of neighbors
    for (auto &edge : G.get_node_mutable(u).adjacent_nodes_mutable()) {
        if (get_residual_capacity(G,u,edge) > 0) {
            min_height = std::min(min_height, height[edge.id()]);
        }
    }

    height[u] = min_height + 1;
    if (height[u] > max_height && height[u] < 2 * G.num_nodes()) {
        max_height = height[u];
    }
}

CapValue solve_max_flow(Graph &G) {
    int n = G.num_nodes();
    int s = 0, t = 1;

    //initialise zero vectors 
    excess = std::vector<CapValue>(n,0);
    height = std::vector<int>(n,0);
    buckets = std::vector<std::vector<int>>(2*n+1);

    // Initialization per algorithm: height(s) = n
    height[s] = n;
    
    excess[s] = Graph::infinite_capacity; // s doesn't become active

    //fill all edges from s
    for (auto &edge : G.get_node_mutable(s).adjacent_nodes_mutable()) {
        if (edge.edge_index() != -1) {
            //actual edges:
            int gamma = edge.capacity();
            update_flow(G,s,edge,gamma);
            excess[edge.id()] += gamma;
            excess[s] -= gamma;
            activate_node(edge.id(), n, t);
        }
    }

    //choose node with max. height (Theorem 4.28.)
    while (max_height >= 0) {
        if (buckets[max_height].empty()) {
            max_height--;
            continue;
        }

        int u = buckets[max_height].back();
        buckets[max_height].pop_back();

        while(excess[u] > 0) {
            //find zulässige edge
            bool pushed = false;
            for (auto &edge : G.get_node_mutable(u).adjacent_nodes_mutable()) {
                if (get_residual_capacity(G, u, edge) > 0 &&
                    height[u] == height[edge.id()] + 1) {
                    push(G, u, edge);
                    if (excess[u] == 0) {
                        pushed = true;
                        break;
                    }
                }
            }

            if (excess[u] > 0) {
                relabel(G,u);
                activate_node(u,n,t);
                break;
            }
        }
    }
    return excess[t];
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    Graph G(argv[1], Graph::directed);

    CapValue max_flow = solve_max_flow(G);

    std::cout << "[maximaler Flusswert: " << max_flow << "]" << std::endl;

    std::vector<std::pair<int, int>> output;
    for (int u = 0; u < G.num_nodes(); ++u) {
        for (const auto &edge : G.get_node(u).adjacent_nodes()) {
            if (edge.edge_index() >= 0 && edge.flow() > 0) {
                output.push_back({edge.edge_index(), edge.flow()});
            }
        }
    }
    std::sort(output.begin(), output.end());

    for (const auto &p : output) {
        std::cout << p.first << " " << p.second << std::endl;
    }
    return 0;
}
