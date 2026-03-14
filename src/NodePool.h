

#ifndef NODEPOOL_H
#define NODEPOOL_H
#include <cassert>
#include <vector>

#include "BbfNode.h"


class NodePool {
private:
    const int M;
    std::vector<BbfNode> nodes;

public:
    std::vector<int> root_timestamps;
    std::vector<int> root_timestamp_validity;


    explicit NodePool(int m) : M(m + 10), root_timestamps(M, -1), root_timestamp_validity(M, -1) {
        nodes.resize(M);
        for (int i = 0; i < M; i++) {
            nodes.at(i).timestamp = i;
        }
    }

    ~NodePool() {
        }

    BbfNode *getNode(int timestamp) {
        assert(timestamp >= 0 && timestamp < M);
        return &nodes[timestamp];
    }
};


#endif