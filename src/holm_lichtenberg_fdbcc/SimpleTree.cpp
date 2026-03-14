

#include "SimpleTree.h"


#include <cassert>
#include <iostream>
#include <ostream>

#include "commons.h"


void SimpleTree::add_node(VId id) {
    assert(id >= 0 && id < N_);
    if (nodes[id].id == id) {
        std::cout << "Node " << id << " already exists" << std::endl;
        throw std::invalid_argument("Node already exists");
    }
    nodes[id].id = id;
}


void SimpleTree::link(VId uid, VId vid) {
    nbrs[vid].insert(uid);
    nbrs[uid].insert(vid);
}

void SimpleTree::cut(VId uid, VId vid) {
    assert(nbrs[uid].contains(vid));
    assert(nbrs[vid].contains(uid));

    nbrs[uid].erase(vid);
    nbrs[vid].erase(uid);
}

bool SimpleTree::is_edge(VId vid, VId uid) {
    return nbrs[vid].contains(uid);
}

TreeNode *SimpleTree::get_node(VId uid) {
    assert(uid >= 0 && uid < N_);
    if (nodes[uid].id != uid) {
        std::cout << "Node " << uid << " not exists" << std::endl;
        throw std::invalid_argument("Node not exists");
    }
    return &nodes[uid];
}

const TreeNode *SimpleTree::get_node(VId uid) const {
    assert(uid >= 0 && uid < N_);
    if (nodes[uid].id != uid) {
        std::cout << "Node " << uid << " not exists" << std::endl;
        throw std::invalid_argument("Node not exists");
    }
    return &nodes[uid];
}