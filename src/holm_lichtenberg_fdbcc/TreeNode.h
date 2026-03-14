

#ifndef TREENODE_H
#define TREENODE_H
#include "my_types.h"


class TreeNode {
public:
    VId id;
    TreeNode *parent;

    TreeNode(VId id, TreeNode *parent)
        : id(id),
          parent(parent) {
    }

    bool is_root() const;
};


#endif