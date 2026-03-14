

#ifndef HOPCROFTTARJANALGORITHM_H
#define HOPCROFTTARJANALGORITHM_H

#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <memory>

class HopcroftTarjan {
public:
    std::vector<std::set<int> > biconnectedComponents;

    explicit HopcroftTarjan(int V);

    void addEdge(int u, int v);

    void tarjanBCC();

    void printBCCs();

private:
    int V;
    std::vector<std::shared_ptr<std::vector<int> > > adj;
    int time;
    std::shared_ptr<std::vector<int> > disc, low;
    std::vector<bool> visited;
    std::stack<std::pair<int, int> > edgeStack;


    void tarjanBCCUtil(int u, int parent);
};


#endif