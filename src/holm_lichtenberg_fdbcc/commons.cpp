

#include "commons.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <strstream>

#include "NotYetMergeSet.h"

void print_bccs(const std::vector<VSetP> &bccs) {
    std::cout << "Biconnected Components:\n";
    for (const auto &bcc: bccs) {
        std::cout << "{ ";
        for (const auto &v: *bcc) {
            std::cout << v << " ";
        }
        std::cout << "}\n";
    }
}

bool holm::contains_bcc(const std::vector<VSetP> &bccs, const std::set<VId> &expected_bcc) {
    for (const auto &bcc: bccs) {
        std::set<VId> current_bcc(bcc->begin(), bcc->end());
        if (current_bcc == expected_bcc) {
            return true;
        }
    }
    return false;
}

VArrP make_VArr() {
    return std::make_shared<std::vector<VId> >();
}

VSetP make_VSet() {
    return std::make_shared<std::set<VId> >();
}


Edge holm::make_edge(VId v1, VId v2) {
    if (v1 > v2) {
        std::swap(v1, v2);
    }
    return std::make_pair(v1, v2);
}


void set_minors(VSetP setA, VSetP setB) {
    VSetP r = make_VSet();
    std::set_difference(setA->begin(), setA->end(),
                        setB->begin(), setB->end(),
                        std::inserter(*r, r->end()));
    setA->clear();
    setA->insert(r->begin(), r->end());
}

std::vector<Edge> read_edges_from_file(const std::string &filename) {
    std::vector<std::pair<int, int> > edges;
    std::ifstream infile(filename);
    std::string line;

    std::getline(infile, line);

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        int source, target;
        char comma;

        if (iss >> source >> comma >> target) {
            edges.emplace_back(source, target);
        }
    }
    return edges;
}

int holm::count_vertices(const std::vector<Edge> &pairs) {
    int max_vid = 0;
    for (auto pair: pairs) {
        max_vid = std::max(max_vid, pair.first);
        max_vid = std::max(max_vid, pair.second);
    }
    return max_vid;
}


int holm::count_vertices(const std::set<Edge> &pairs) {
    int max_vid = 0;
    for (auto pair: pairs) {
        max_vid = std::max(max_vid, pair.first);
        max_vid = std::max(max_vid, pair.second);
    }
    return max_vid;
}

std::string str(Wedge w) {
    std::stringstream ss;
    ss << "(" << w.x
            << ", " << w.pivot << ", " << w.z << ")";
    return ss.str();
}

std::string str(Edge e) {
    std::stringstream ss;
    ss << "(" << e.first << ", " << e.second << ")";
    return ss.str();
}

std::string str(const VArrP &path) {
    if (path->empty()) {
        return "";
    }
    std::stringstream ss;
    ss << path->front();
    for (int i = 1; i < path->size(); i++) {
        ss << "-" << path->at(i);
    }
    return ss.str();
}

std::string str(const VSetP &nodes) {
    std::stringstream ss;
    ss << "{ ";
    for (auto v: *nodes) {
        ss << v << " ";
    }
    ss << "}";
    return ss.str();
}