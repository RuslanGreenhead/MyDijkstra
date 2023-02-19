//
// Graph class utilities
//

#pragma once
#include "Graph.h"
#include "Fileswork.hpp" // Matrix.h included
#include <vector>
#include <list>
#include <algorithm>


using node_name_t = size_t;
using node_data_t = bool;
using weight_t = double;
using graph_t = Graph<node_name_t, node_data_t, weight_t>;
using matrix_t = Matrix<weight_t>;
using route_t = std::list<node_name_t>;
using text_mode_t = TextMode<double>;


// Print graph
template<typename MyGraph>
void print(const MyGraph& graph) {
    if (graph.empty()) {
        std::cout << "> This graph is empty!" << std::endl;
        return;
    }
    std::cout << "> Number of nodes: " << graph.size() << std::endl;
    for (const auto& [key, node] : graph) {
        std::cout << '[' << key << "] stores: " << node.value()
                  << " and matches with:" << std::endl;
        for (const auto& [key_1, weight] : node)
            std::cout << "\t[" << key_1 << "]\t with weight: "
                      << weight << std::endl;
    }
}


// Read graph from matrix from txt file
graph_t read_graph(const char *filename) {
    matrix_t matr = TextMode<weight_t>::read(filename);

    if ((matr.num_rows() != matr.num_cols())) {
        throw runtime_error("Adjacency matrix is not squared!");
    }

    graph_t res_graph;
    for (int i = 0; i < matr.num_rows(); ++i) res_graph.insert_node(i, node_data_t());
    for (int i = 0; i < matr.num_rows(); ++i) {
        for (int j = 0; j < matr.num_cols(); ++j) {
            if (matr(i, j) != 0) res_graph.insert_edge({i, j}, matr(i, j));
        }
    }
    return res_graph;
}


// Helper function for deijkstra algorithm
size_t min_not_visited(const std::vector<weight_t>& vec, std::vector<bool> labels){
    weight_t min;
    size_t min_index;

    bool all_visited = true;
    for(int i = 0; i < vec.size(); ++i){
        if(!labels[i]){
            min = vec[i];
            min_index = i;
            all_visited = false;
        }
    }
    if(all_visited) return -1;

    for(int i = 0; i < vec.size(); ++i){
        if((vec[i] < min) && (!labels[i])){
            min = vec[i];
            min_index = i;
        }
    }
    return min_index;
}


// Deijkstra algorithm
std::pair<weight_t, route_t> deijkstra(const graph_t& graph, node_name_t key_from, node_name_t key_to){
    std::vector<weight_t> ways(graph.size(), INT_MAX);
    std::vector<bool> labels(graph.size(), false); // labels[A] == false -> node A is not visited
    std::vector<node_name_t> prev_points(graph.size(), -1);

    ways[key_from] = 0;

    while(true) {
        size_t cur_index = min_not_visited(ways, labels);
        if (cur_index == -1) break;
        for (auto it = graph.get_node(cur_index).begin(); it != graph.get_node(cur_index).end(); ++it) {
            if(it->second < 0) {throw runtime_error("Weight less than zero detected!");}
            if (ways[cur_index] + it->second < ways[it->first]) {
                ways[it->first] = ways[cur_index] + it->second;
                prev_points[it->first] = cur_index;
            }
        }
        labels[cur_index] = true;
    }

    if(prev_points[key_to] == -1){
        return std::pair{ways[key_to], std::list<node_name_t>(1, -1)};
    }

    std::list<node_name_t> res_route;
    size_t i = prev_points[key_to];
    while(i != key_from){
        res_route.push_front(i);
        i = prev_points[i];
    }
    res_route.push_front(key_from);
    res_route.push_back(key_to);

    return std::pair{ways[key_to], res_route};
}
