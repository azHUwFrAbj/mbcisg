

#ifndef UTILS_H
#define UTILS_H
#include <set>
#include <vector>
#include <queue>
#include <chrono>
#include <map>
#include <unordered_map>
#include <climits>

#define RECOMPUTE
#include "PVec.h"

using VId = int;
using VecVId = PVec<VId>;
using BCC = std::set<VId>;
using BCCs = std::vector<std::shared_ptr<BCC> >;

using Edge = std::pair<VId, VId>;
using VecEdge = PVec<Edge>;

using TEdge = std::pair<Edge, int>;
using EdgeCyclePairs = std::vector<std::pair<TEdge, std::vector<TEdge> > >;


using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;


Edge make_edge(VId v1, VId v2);

template<class EgContainer>
int get_max_vid(const EgContainer &edges) {
    int max_vid = 0;
    for (auto pair: edges) {
        max_vid = std::max(max_vid, pair.first);
        max_vid = std::max(max_vid, pair.second);
    }
    return max_vid;
}

template<class EgContainer>
int count_vertices(const EgContainer &edges) {
    std::set<VId> vs;;
    for (auto pair: edges) {
        vs.insert(pair.first);
        vs.insert(pair.second);
    }
    return (int) vs.size();
}

const std::string TOY_GRAPH_CSV = "../datasets/toy_graph_edges.csv";
const std::string LASTFM_CSV = "../datasets/lastfm_asia_edges.csv";
const std::string DEEZER_CSV = "../datasets/deezer_europe_edges.csv";

#endif