

#ifndef EDGEINFO_H
#define EDGEINFO_H
#include <unordered_map>

#include "my_types.h"
#include "commons.h"


class EdgeInfo {
private:
    std::unordered_map<Edge, int, EdgeHash> levelM;
    std::unordered_map<Edge, int, EdgeHash> coverM;
    std::unordered_map<Edge, Edge, EdgeHash> repM;

public:
    const static int DEFAULT_WEIGHT = -1;
    const static Edge INVALID;

    double get_index_size_in_KB();

    int get_level(VId v1, VId v2) {
        Edge edge = holm::make_edge(v1, v2);
        return levelM.contains(edge) ? levelM[edge] : DEFAULT_WEIGHT;
    }

    int get_cover_level(VId v1, VId v2) {
        Edge edge = holm::make_edge(v1, v2);
        return coverM.contains(edge) ? coverM[edge] : DEFAULT_WEIGHT;
    }

    bool is_rep_edge(VId v1, VId v2) const {
        Edge edge = holm::make_edge(v1, v2);
        return repM.contains(edge) && repM.at(edge) != INVALID;
    }

    Edge get_rep_edge(VId v1, VId v2) {
        Edge edge = holm::make_edge(v1, v2);
        return repM.contains(edge) ? repM[edge] : INVALID;
    };

    void set_level(VId v1, VId v2, int l) {
        Edge edge = holm::make_edge(v1, v2);
        levelM[edge] = l;
    }

    void set_cover_level(VId v1, VId v2, int c) {
        Edge edge = holm::make_edge(v1, v2);
        coverM[edge] = c;
    }

    void set_rep_edge(VId v1, VId v2, Edge e) {
        Edge edge = holm::make_edge(v1, v2);
        repM[edge] = e;
    }
};

#endif