

#include "../AUF.h"

#include <cassert>


AUF::AUF_Node &AUF::get_node(BBF::Node *node) {
    int index = node->timestamp - t_s_;
    assert(index >= 0 || index <= auf_nodes.size());
    return auf_nodes.at(index);
}

void AUF::set_node(BBF::Node *node, AUF_Node value) {
    get_node(node) = value;
}

bool AUF::is_in(BBF::Node *node) {
    return get_node(node).anchor != nullptr;
}

void AUF::make_set(BBF::Node *x) {
    set_node(x, {x, x, 0});
}

BBF::Node *AUF::find(BBF::Node *x) {
    AUF_Node &node = get_node(x);
    if (node.parent != x) {
        node.parent = find(node.parent);
    }
    return node.parent;
}

void AUF::union_sets(BBF::Node *x, BBF::Node *y) {
    BBF::Node *xRoot = find(x);
    BBF::Node *yRoot = find(y);

    if (xRoot == yRoot) return;

    AUF_Node &xNode = get_node(xRoot);
    AUF_Node &yNode = get_node(yRoot);

    if (xNode.rank < yNode.rank) {
        xNode.parent = yRoot;
    } else if (xNode.rank > yNode.rank) {
        yNode.parent = xRoot;
    } else {
        yNode.parent = xRoot;
        xNode.rank += 1;
    }

    BBF::Node *x_anchor = xNode.anchor;
    BBF::Node *y_anchor = yNode.anchor;
    BBF::Node *x_root = find(x);
    get_node(x_root).anchor = x_anchor->timestamp <= y_anchor->timestamp ? x_anchor : y_anchor;
}


BBF::Node *AUF::get_anchor(BBF::Node *x) {
    auto x_root = find(x);
    return get_node(x_root).anchor;
}