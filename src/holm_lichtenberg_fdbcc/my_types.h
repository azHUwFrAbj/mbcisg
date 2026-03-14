

#ifndef MY_TYPES_H
#define MY_TYPES_H
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using VId = int;

using VSetP = std::shared_ptr<std::set<VId> >;

using VArrP = std::shared_ptr<std::vector<VId> >;

using Edge = std::pair<VId, VId>;

struct EdgeHash {
    std::size_t operator()(const Edge &edge) const {
        auto h1 = std::hash<VId>{}(edge.first);
        auto h2 = std::hash<VId>{}(edge.second);
        return h1 ^ (h2 << 1);
    }
};

struct Wedge {
    int x;
    int pivot;
    int z;
};

#endif