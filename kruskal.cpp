#include <iostream>
#include "graph.h"
#include <vector>

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

    



    return 0;
}