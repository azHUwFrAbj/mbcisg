

#ifndef BASICTREE_H
#define BASICTREE_H
#include <map>
#include <vector>

#include "my_types.h"
#include "TreeNode.h"


class TreeNode;

class BasicTree {
private:
    const int N_;
    std::vector<TreeNode> nodes;

public:
    BasicTree() = delete;

    explicit BasicTree(int N) : N_(N + 1), nodes(N_, {-1, nullptr}) {
        };

public:
    void add_node(VId id);

    void link(VId uid, VId vid);

    void cut(VId uid, VId vid);

    bool is_connected(VId uid, VId vid);

    VArrP find_path(int uid, int vid);

    bool is_edge(VId vid, VId uid);

    TreeNode *get_node(VId uid);

    const TreeNode *get_node(VId uid) const;

    std::pair<VArrP, VSetP> find_path_vset(int uid, int vid);

private:
    void link_roots(VId uid, VId vid);

    void reroot(VId uid);

    bool is_root(VId uid) const;
};


#endif