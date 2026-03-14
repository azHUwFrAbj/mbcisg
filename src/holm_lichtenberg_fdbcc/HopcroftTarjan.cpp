

#include "HopcroftTarjan.h"

#include <unordered_set>

using namespace std;

HopcroftTarjan::HopcroftTarjan(int V) : V(V) {
    adj.resize(V + 1);
    disc = make_shared<std::vector<int> >(V + 1, -1);
    low = make_shared<std::vector<int> >(V + 1, -1);
    visited.resize(V + 1, false);
    time = 0;
    for (int i = 0; i < V + 1; i++) {
        adj[i] = make_shared<std::vector<int> >();
    }
}


void HopcroftTarjan::addEdge(int u, int v) {
    adj[u]->push_back(v);
    adj[v]->push_back(u);
}

void HopcroftTarjan::tarjanBCCUtil(int u, int parent) {
    visited[u] = true;
    (*disc)[u] = (*low)[u] = time++;
    int children = 0;

    for (int v: *adj[u]) {
        if (!visited[v]) {
            edgeStack.push({u, v});
            children++;
            tarjanBCCUtil(v, u);

            (*low)[u] = min((*low)[u], (*low)[v]);

            if ((parent == -1 && children > 1) || (parent != -1 && (*low)[v] >= (*disc)[u])) {
                set<int> component;
                while (true) {
                    auto edge = edgeStack.top();
                    edgeStack.pop();
                    component.insert(edge.first);
                    component.insert(edge.second);
                    if (edge.first == u && edge.second == v) break;
                }
                biconnectedComponents.push_back(component);
            }
        } else if (v != parent && (*disc)[v] < (*disc)[u]) {
            edgeStack.push({u, v});
            (*low)[u] = min((*low)[u], (*disc)[v]);
        }
    }
}

void HopcroftTarjan::tarjanBCC() {
    for (int i = 1; i <= V; i++) {
        if (!visited[i]) {
            tarjanBCCUtil(i, -1);

            if (!edgeStack.empty()) {
                set<int> component;
                while (!edgeStack.empty()) {
                    auto edge = edgeStack.top();
                    edgeStack.pop();
                    component.insert(edge.first);
                    component.insert(edge.second);
                }
                biconnectedComponents.push_back(component);
            }
        }
    }
}

void HopcroftTarjan::printBCCs() {
    cout << "Biconnected Components:" << endl;
    for (const auto &component: biconnectedComponents) {
        for (int node: component) {
            cout << node << " ";
        }
        cout << endl;
    }
}