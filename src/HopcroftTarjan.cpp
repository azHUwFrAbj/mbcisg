

#include "HopcroftTarjan.h"

using namespace std;

HopcroftTarjan::HopcroftTarjan(int V) : V(V), vertex_label(V + 1) {
    adj.resize(V + 1);
    disc = std::make_shared<std::vector<int> >(V + 1, -1);
    low = std::make_shared<std::vector<int> >(V + 1, -1);

    edgeStack = make_shared<std::vector<Edge> >();
    visited = std::make_shared<std::vector<bool> >(static_cast<std::vector<bool>::size_type>(V + 1), false);
    time = 0;
    for (int i = 0; i < V + 1; i++) {
        adj.at(i) = make_shared<std::vector<int> >();
    }
}


void HopcroftTarjan::add_edge(int u, int v) {
    adj.at(u)->push_back(v);
    adj.at(v)->push_back(u);
}

void HopcroftTarjan::util_func(int u, int parent) {
    visited->at(u) = true;
    disc->at(u) = low->at(u) = time++;
    int children = 0;

    for (int v: *(adj.at(u))) {
        if (!visited->at(v)) {
            edgeStack->push_back({u, v});
            children++;
            util_func(v, u);

            low->at(u) = min(low->at(u), low->at(v));

            if ((parent == -1 && children > 1) || (parent != -1 && low->at(v) >= disc->at(u))) {
                shared_ptr<set<int> > component = make_shared<set<int> >();
                while (true) {
                    auto edge = edgeStack->back();
                    edgeStack->pop_back();

                    component->insert(edge.first);
                    component->insert(edge.second);

                    if (edge.first == u && edge.second == v) break;
                }

                biconnectedComponents.push_back(component);
            }
        } else if (v != parent && disc->at(v) < disc->at(u)) {
            edgeStack->push_back({u, v});
            low->at(u) = min(low->at(u), disc->at(v));
        }
    }
}

void HopcroftTarjan::tarjan_bcc() {
    tarjan_bcc_iterative();

    }

void HopcroftTarjan::print_bccs() {
    cout << "Biconnected Components:" << endl;
    for (const auto &component: biconnectedComponents) {
        for (int node: *component) {
            cout << node << " ";
        }
        cout << endl;
    }
}

void HopcroftTarjan::run_and_get_bccs(std::vector<std::vector<VId> > &bccs) {
    bccs.clear();

    vector<int> in_bcc(adj.size(), -1);

    for (int i = 1; i <= V; ++i) {
        if (visited->at(i)) continue;

        std::stack<Frame> stk;
        stk.emplace(i, -1, adj.at(i));

        while (!stk.empty()) {
            Frame &frame = stk.top();
            int u = frame.u;
            int parent = frame.parent;

            if (frame.first_visit) {
                visited->at(u) = true;
                disc->at(u) = low->at(u) = time++;
                frame.first_visit = false;
            }

            bool advanced = false;
            while (frame.nbr_it != frame.nbr_end) {
                int v = *frame.nbr_it++;
                if (v == parent) continue;

                if (!visited->at(v)) {
                    edgeStack->emplace_back(u, v);
                    frame.children++;
                    stk.emplace(v, u, adj.at(v));
                    advanced = true;
                    break;
                }

                if (disc->at(v) < disc->at(u)) {
                    edgeStack->emplace_back(u, v);
                    low->at(u) = std::min(low->at(u), disc->at(v));
                }
            }

            if (advanced) continue;

            for (int v: *adj.at(u)) {
                if (v != parent && visited->at(v) && disc->at(v) > disc->at(u))
                    low->at(u) = std::min(low->at(u), low->at(v));
            }

            if ((parent == -1 && frame.children > 1) ||
                (parent != -1 && low->at(u) >= disc->at(parent))) {
                vector<VId> component;
                while (!edgeStack->empty()) {
                    auto [a, b] = edgeStack->back();
                    edgeStack->pop_back();

                    if (in_bcc[a] != bccs.size()) {
                        component.push_back(a);
                        in_bcc[a] = bccs.size();
                    }
                    if (in_bcc[b] != bccs.size()) {
                        component.push_back(b);
                        in_bcc[b] = bccs.size();
                    }
                    if ((a == parent && b == u) || (a == u && b == parent)) break;
                }
                if (!component.empty()) {
                    bccs.push_back(component);
                }
            }

            stk.pop();
        }
    }
}

void HopcroftTarjan::run_label_based() {
    for (int i = 1; i <= V; ++i) {
        if (visited->at(i)) continue;

        std::stack<Frame> stk;
        stk.emplace(i, -1, adj.at(i));

        while (!stk.empty()) {
            Frame &frame = stk.top();
            int u = frame.u;
            int parent = frame.parent;

            if (frame.first_visit) {
                visited->at(u) = true;
                disc->at(u) = low->at(u) = time++;
                frame.first_visit = false;
            }

            bool advanced = false;
            while (frame.nbr_it != frame.nbr_end) {
                int v = *frame.nbr_it++;
                if (v == parent) continue;

                if (!visited->at(v)) {
                    edgeStack->emplace_back(u, v);
                    frame.children++;
                    stk.emplace(v, u, adj.at(v));
                    advanced = true;
                    break;
                }

                if (disc->at(v) < disc->at(u)) {
                    edgeStack->emplace_back(u, v);
                    low->at(u) = std::min(low->at(u), disc->at(v));
                }
            }

            if (advanced) continue;

            for (int v: *adj.at(u)) {
                if (v != parent && visited->at(v) && disc->at(v) > disc->at(u))
                    low->at(u) = std::min(low->at(u), low->at(v));
            }

            if ((parent == -1 && frame.children > 1) ||
                (parent != -1 && low->at(u) >= disc->at(parent))) {
                while (!edgeStack->empty()) {
                    auto [a, b] = edgeStack->back();
                    edgeStack->pop_back();
                    vertex_label.at(a) = label_id;
                    vertex_label.at(b) = label_id;
                    if ((a == parent && b == u) || (a == u && b == parent)) break;
                }
                label_id++;
                }

            stk.pop();
        }
    }
}

void HopcroftTarjan::get_bcc_of_vertex(std::vector<std::vector<VId> > &bccs, VId query_vertex) {
    bccs.clear();

    vector<int> in_bcc(adj.size(), -1);

    int i = query_vertex;

    std::stack<Frame> stk;
    stk.emplace(i, -1, adj.at(i));

    while (!stk.empty()) {
        Frame &frame = stk.top();
        int u = frame.u;
        int parent = frame.parent;

        if (frame.first_visit) {
            visited->at(u) = true;
            disc->at(u) = low->at(u) = time++;
            frame.first_visit = false;
        }

        bool advanced = false;
        while (frame.nbr_it != frame.nbr_end) {
            int v = *frame.nbr_it++;
            if (v == parent) continue;

            if (!visited->at(v)) {
                edgeStack->emplace_back(u, v);
                frame.children++;
                stk.emplace(v, u, adj.at(v));
                advanced = true;
                break;
            }

            if (disc->at(v) < disc->at(u)) {
                edgeStack->emplace_back(u, v);
                low->at(u) = std::min(low->at(u), disc->at(v));
            }
        }

        if (advanced) continue;

        for (int v: *adj.at(u)) {
            if (v != parent && visited->at(v) && disc->at(v) > disc->at(u))
                low->at(u) = std::min(low->at(u), low->at(v));
        }

        bool query_vertex_in = false;

        if ((parent == -1 && frame.children > 1) ||
            (parent != -1 && low->at(u) >= disc->at(parent))) {
            vector<VId> component;
            while (!edgeStack->empty()) {
                auto [a, b] = edgeStack->back();
                edgeStack->pop_back();
                if (in_bcc[a] != bccs.size()) {
                    in_bcc[a] = bccs.size();
                    component.push_back(a);
                }
                if (in_bcc[b] != bccs.size()) {
                    in_bcc[b] = bccs.size();
                    component.push_back(b);
                }
                if (a == query_vertex || b == query_vertex) query_vertex_in = true;

                if ((a == parent && b == u) || (a == u && b == parent)) break;
            }
            if (!component.empty() && query_vertex_in) {
                bccs.push_back(component);
            }
        }

        stk.pop();
    }
    }

void HopcroftTarjan::reset_visited() {
    for (size_t i = 0; i < visited->size(); ++i) {
        visited->at(i) = false;
    }
}

void HopcroftTarjan::tarjan_bcc_iterative() {
    for (int i = 1; i <= V; ++i) {
        if (visited->at(i)) continue;

        std::stack<Frame> stk;
        stk.emplace(i, -1, adj.at(i));

        while (!stk.empty()) {
            Frame &frame = stk.top();
            int u = frame.u;
            int parent = frame.parent;

            if (frame.first_visit) {
                visited->at(u) = true;
                disc->at(u) = low->at(u) = time++;
                frame.first_visit = false;
            }

            bool advanced = false;
            while (frame.nbr_it != frame.nbr_end) {
                int v = *frame.nbr_it++;
                if (v == parent) continue;

                if (!visited->at(v)) {
                    edgeStack->emplace_back(u, v);
                    frame.children++;
                    stk.emplace(v, u, adj.at(v));
                    advanced = true;
                    break;
                }

                if (disc->at(v) < disc->at(u)) {
                    edgeStack->emplace_back(u, v);
                    low->at(u) = std::min(low->at(u), disc->at(v));
                }
            }

            if (advanced) continue;

            for (int v: *adj.at(u)) {
                if (v != parent && visited->at(v) && disc->at(v) > disc->at(u))
                    low->at(u) = std::min(low->at(u), low->at(v));
            }

            if ((parent == -1 && frame.children > 1) ||
                (parent != -1 && low->at(u) >= disc->at(parent))) {
                std::shared_ptr<std::set<int> > component = std::make_shared<std::set<int> >();
                while (!edgeStack->empty()) {
                    auto [a, b] = edgeStack->back();
                    edgeStack->pop_back();
                    component->insert(a);
                    component->insert(b);
                    if ((a == parent && b == u) || (a == u && b == parent)) break;
                }
                if (component->size() > 0) {
                    biconnectedComponents.push_back(component);
                }
            }

            stk.pop();
        }
    }
}