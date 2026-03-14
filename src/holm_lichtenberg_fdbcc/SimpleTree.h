

#ifndef SIMPLETREE_H
#define SIMPLETREE_H

#include "my_types.h"
#include "TreeNode.h"


class TreeNode;

class SimpleTree {
private:
    const int N_;
    std::vector<TreeNode> nodes;

public:
    std::vector<std::set<VId> > nbrs;

public:
    SimpleTree() = delete;

    SimpleTree(int N, bool init_nodes) : N_(N + 1), nodes(N_, {-1, nullptr}), nbrs(N_) {
        if (init_nodes) {
            for (int i = 0; i < N_; i++) {
                nodes[i].id = i;
            }
        }
        };

public:
    void add_node(VId id);

    void link(VId uid, VId vid);

    void cut(VId uid, VId vid);

    bool is_edge(VId vid, VId uid);

    TreeNode *get_node(VId uid);

    const TreeNode *get_node(VId uid) const;
};


#endif