

#ifndef HOPCROFTTARJANALGORITHM_H
#define HOPCROFTTARJANALGORITHM_H

#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <algorithm>

#include "utils.h"

class HopcroftTarjan {
    struct Frame {
        int u;
        int parent;
        bool first_visit;
        std::vector<int>::iterator nbr_it;
        std::vector<int>::iterator nbr_end;
        int children = 0;

        Frame(int u, int parent, const std::shared_ptr<std::vector<int> > &neighbors)
            : u(u),
              parent(parent),
              first_visit(true),
              nbr_it(neighbors->begin()),
              nbr_end(neighbors->end()) {
        }
    };

public:
    BCCs biconnectedComponents;


    explicit HopcroftTarjan(int V);

    void add_edge(int u, int v);

    void tarjan_bcc();

    void print_bccs();

    void run_and_get_bccs(std::vector<std::vector<VId> > &bccs);

    void run_label_based();

    void get_bcc_of_vertex(std::vector<std::vector<VId> > &bbcs, VId query_vertex);

    void reset_visited();

private:
    int label_id = 0;
    std::vector<VId> vertex_label;

    int V;
    std::vector<std::shared_ptr<std::vector<int> > > adj;
    int time;
    std::shared_ptr<std::vector<int> > disc, low;
    std::shared_ptr<std::vector<bool> > visited;
    std::shared_ptr<std::vector<Edge> > edgeStack;

    void util_func(int u, int parent);

    void tarjan_bcc_iterative();
};


#endif