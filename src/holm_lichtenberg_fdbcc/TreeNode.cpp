

#include "TreeNode.h"

bool TreeNode::is_root() const {
    return parent == nullptr;
}