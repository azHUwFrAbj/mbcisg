

#ifndef UNIONFIND_H
#define UNIONFIND_H
#include <regex>

#include "my_types.h"


class DisjointSet {
    std::vector<int> parent;
    std::vector<int> size;

public:
    explicit DisjointSet(int maxSize) {
        parent.resize(maxSize);
        size.resize(maxSize);
        for (int i = 0; i < maxSize; i++) {
            parent[i] = i;
            size[i] = 1;
        }
    }

    int find_set(int v) {
        if (v == parent[v])
            return v;

        return parent[v] = find_set(parent[v]);
    }

    void union_set(int a, int b) {
        a = find_set(a);
        b = find_set(b);
        if (a != b) {
            if (size[a] < size[b])
                std::swap(a, b);
            parent[b] = a;
            size[a] += size[b];
        }
    }

    std::vector<std::vector<int> > get_all_sets() {
        std::unordered_map<int, std::vector<int> > sets;

        for (int i = 0; i < parent.size(); i++) {
            int root = find_set(i);
            sets[root].push_back(i);
        }

        std::vector<std::vector<int> > result;
        for (const auto &[root, members]: sets) {
            result.push_back(members);
        }

        return result;
    }
};


#endif