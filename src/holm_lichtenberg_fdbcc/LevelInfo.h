

#ifndef LEVELINFO_H
#define LEVELINFO_H
#include <unordered_map>

#include "commons.h"
#include "my_types.h"


class LevelInfo {
private:
    std::unordered_map<int, VSetP> vertices_at_level;
    std::unordered_map<int, std::unordered_map<int, VSetP> > neighbours_at_level;

public:
    double get_index_size_in_KB();

    LevelInfo() = delete;

    explicit LevelInfo(int lmax) {
        for (int i = 0; i <= lmax; i++) {
            VSetP vertices = make_VSet();
            vertices_at_level[i] = vertices;
            neighbours_at_level[i] = {};
        }
    }

    int getVerticesNumber(int l) const {
        return (int) vertices_at_level.at(l)->size();
    }

    void addEdge(VId u, VId v, int l) {
        vertices_at_level.at(l)->insert(u);
        vertices_at_level.at(l)->insert(v);
        auto uNb = getNbrs(u, l);
        auto vNb = getNbrs(v, l);
        uNb->insert(v);
        vNb->insert(u);
    }

    VSetP getNbrs(VId v, int l) {
        if (!neighbours_at_level.at(l).contains(v)) {
            VSetP nbr = make_VSet();
            neighbours_at_level.at(l)[v] = nbr;
        }
        return neighbours_at_level.at(l).at(v);
    }

    void removeEdge(VId u, VId v, int l) {
        auto uNb = getNbrs(u, l);
        auto vNb = getNbrs(v, l);
        uNb->erase(v);
        vNb->erase(u);
        if (uNb->empty()) {
            vertices_at_level.at(l)->erase(u);
        }
        if (vNb->empty()) {
            vertices_at_level.at(l)->erase(v);
        }
    }

    bool contains(VId v, int l) {
        return vertices_at_level[l]->contains(v);
    }
};


#endif