#include <iostream>
#include "Graph.h"
#include "Matrix.h"
#include "Graph_utilities.h"
#include <string.h>

std::tuple<const char*, size_t, size_t> parse_main_input(const char* args[]){
    if((strcmp(args[1], "-file") != 0) || (strcmp(args[3], "-from") != 0) || (strcmp(args[5], "-to") != 0)){
        throw runtime_error("Invalid input format");
    }
    const char* filename = args[2];
    size_t key_from = atoi(args[4]);
    size_t key_to = atoi(args[6]);
    return std::tuple{filename, key_from, key_to};
}

int main(int arg_count, const char* arg_vars[]) {

    if(arg_count != 7){throw runtime_error("Invalid input format");}

    auto [filename, key_from, key_to] = parse_main_input(arg_vars);

    /*Matrix<double> m = {{0, 0, 7, 4},
                     {2, 0, 0, 0},
                     {0, 0, 0, 0},
                     {0, 0, 5, 0},};
    TextMode<double>::write("gr2.txt", m);*/

    graph_t gr1 = read_graph(filename);

    auto [weight, route] =  deijkstra(gr1, key_from, key_to);

    if(route == std::list<node_name_t>(1, -1)) {
        std::cout << "no way" << std::endl;
        return 0;
    }

    std::cout << "weight: " << weight << std::endl;
    std::cout << "route: ";
    for(auto el : route) std::cout << el << " ";

    return 0;
}
