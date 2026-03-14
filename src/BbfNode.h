

#ifndef BBFNODE_H
#define BBFNODE_H
#include <set>

#include "my_types.h"

#define USING_SET_FOR_CHILDREN

struct BbfNode {
    Edge gamma;
    int timestamp;
    BbfNode *primary_parent = nullptr;
    std::set<BbfNode *> out_nbrs;

#ifdef USING_SET_FOR_CHILDREN
    std::set<BbfNode *> children;
#else
    std::vector<BbfNode *> children;
#endif

    bool is_deleted = false;
    bool simple_versatile_mark = false;
    int ss_timestamp;
    int ss_ancestor_timestamp;

    int cache_validity = -1;
    std::vector<VId> cache_bcc;
};

struct Compare_Timestamp {
    bool operator()(const BbfNode *l, const BbfNode *r) const { return l->timestamp < r->timestamp; }
};


#endif