#include <iostream>
#include "graph.h"
#include <vector>
#include <tuple>
#include <algorithm>

void make_set(std::vector<int>& parents, std::vector<int>& depth, int x){
    parents[x] = x;
    depth[x] = 0;
}

int find(std::vector<int>& parents, int x){
    if(parents[x] != x){
        parents[x]=find(parents, parents[x]);
    }

    return parents[x];
}

void Union(std::vector<int>& parents, std::vector<int>& depth, int x, int y){
    int rep_x = find(parents, x);
    int rep_y = find(parents, y);

    if(depth[rep_x] > depth[rep_y]){
        parents[rep_y] = rep_x;
    }else{
        parents[rep_x] = rep_y;

        if(depth[rep_x] == depth[rep_y]){
            depth[rep_y] ++;
        }
    }

}



int main(){

    Graph G("inst_1.txt", Graph::undirected);
    std::vector<int> parents(G.num_nodes(),0);
    std::vector<int> depth(G.num_nodes(),0);
    double total_weight = 0;
    Graph F(G.num_nodes(),Graph::undirected);
    int count_edges_in_F;


    std::vector<std::tuple<double, Graph::NodeId, Graph::NodeId>> edge_list;


    for(int i = 0; i< G.num_nodes();i++){

        make_set(parents, depth, i);

    }


    for(Graph::NodeId i = 0; i < G.num_nodes(); i++){

        const Graph::Node& node = G.get_node(i);

        for(const Graph::Neighbor& neighbor : node.adjacent_nodes()){

            if(i < neighbor.id()){
                edge_list.push_back(std::make_tuple(neighbor.edge_weight(), i, neighbor.id()));
            }

        }
    }

    std::sort(edge_list.begin(),edge_list.end());

    

    for(int i = 0; i< edge_list.size(); i++ ){

        double weight = std::get<0>(edge_list[i]);
        Graph::NodeId x = std::get<1>(edge_list[i]);
        Graph::NodeId y = std::get<2>(edge_list[i]);
        

        if(find(parents, x) != find(parents, y)){

            Union(parents, depth, x, y);
            F.add_edge(x,y,weight);
            total_weight += weight;
            count_edges_in_F++;
        }


    }


    if(count_edges_in_F = G.num_nodes()-1){
        std::cout << "The following is a minimum spanning tree with weight:" << total_weight << std::endl; 
        F.print();
    }else{
        std::cout << "G does not have a minimum spanning Tree" << std::endl;
    }



    return 0;
}