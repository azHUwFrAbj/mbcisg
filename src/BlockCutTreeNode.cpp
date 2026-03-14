

#include "../BlockCutTreeNode.h"

BlockCutTreeNode *BlockCutTreeNode::new_round_node(int id) {
    return new BlockCutTreeNode(block_cut_tree::ROUND, id);
}

BlockCutTreeNode *BlockCutTreeNode::new_square_node(int id) {
    return new BlockCutTreeNode(block_cut_tree::SQUARE, id);
}

bool BlockCutTreeNode::is_square() const {
    return type_ == block_cut_tree::SQUARE;
}

std::unordered_set<int> &BlockCutTreeNode::get_children() {
    if (childrens_.empty() && !children_buffer_.empty()) {
        childrens_.insert(children_buffer_.begin(), children_buffer_.end());
        children_buffer_.clear();
    }
    return childrens_;
}

BlockCutTreeNode::BlockCutTreeNode(const block_cut_tree::Type &type, const int &id) : type_(type), id_(id) {
}