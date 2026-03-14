

#ifndef BLOCKCUTTREENODE_H
#define BLOCKCUTTREENODE_H
#include <unordered_set>
#include <vector>

namespace block_cut_tree {
    enum Type { SQUARE, ROUND };
}

class BlockCutTreeNode {
public:
    block_cut_tree::Type type_;
    const int id_;
    BlockCutTreeNode *parent_ = nullptr;

    std::vector<int> children_buffer_;

    static BlockCutTreeNode *new_round_node(int id);

    static BlockCutTreeNode *new_square_node(int id);

    bool is_square() const;

    std::unordered_set<int> &get_children();

    std::unordered_set<int> childrens_;

private:
    BlockCutTreeNode(const block_cut_tree::Type &type, const int &id);
};


#endif