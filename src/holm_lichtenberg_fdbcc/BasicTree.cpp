

#include "BasicTree.h"

#include <cassert>
#include <iostream>
#include <ostream>

#include "commons.h"


void BasicTree::add_node(VId id) {
    assert(id >= 0 && id < N_);
    if (nodes[id].id == id) {
        std::cout << "Node " << id << " already exists" << std::endl;
        throw std::invalid_argument("Node already exists");
    }
    nodes[id].id = id;
}

void BasicTree::link_roots(VId uid, VId vid) {
    auto u = get_node(uid);
    auto v = get_node(vid);
    if (u->is_root() && v->is_root()) {
        u->parent = v;
        return;
    }
    throw std::invalid_argument("Nodes are not roots");
}

void BasicTree::link(VId uid, VId vid) {
    if (is_connected(uid, vid)) {
        std::cerr << "trying to link connected nodes:" << uid << " and " << vid << std::endl;
        return;
    }
    reroot(uid);
    reroot(vid);
    link_roots(uid, vid);
}

void BasicTree::cut(VId uid, VId vid) {
    auto u = get_node(uid);
    auto v = get_node(vid);
    if (u->parent == v) {
        u->parent = nullptr;
        return;
    }
    if (v->parent == u) {
        v->parent = nullptr;
        return;
    }
    throw std::invalid_argument("Nodes are not linked");
}

void BasicTree::reroot(VId uid) {
    auto u = get_node(uid);
    if (u->is_root()) {
        return;
    }

    auto v = u->parent;
    reroot(v->id);
    v->parent = u;
    u->parent = nullptr;
}

bool BasicTree::is_root(VId uid) const {
    return get_node(uid)->is_root();
}

bool BasicTree::is_connected(VId uid, VId vid) {
    return !find_path(uid, vid)->empty();
}

VArrP BasicTree::find_path(VId uid, VId vid) {
    reroot(vid);
    auto path = std::make_shared<std::vector<int> >();
    auto u = get_node(uid);
    while (u != nullptr) {
        path->push_back(u->id);
        if (u->id == vid) {
            return path;
        }
        u = u->parent;
    }
    path->clear();
    return path;
}

bool BasicTree::is_edge(VId vid, VId uid) {
    return (get_node(vid)->parent != nullptr && get_node(vid)->parent->id == uid) ||
           (get_node(uid)->parent != nullptr && get_node(uid)->parent->id == vid);
}

TreeNode *BasicTree::get_node(VId uid) {
    assert(uid >= 0 && uid < N_);
    if (nodes[uid].id != uid) {
        std::cout << "Node " << uid << " not exists" << std::endl;
        throw std::invalid_argument("Node not exists");
    }
    return &nodes[uid];
}

const TreeNode *BasicTree::get_node(VId uid) const {
    assert(uid >= 0 && uid < N_);
    if (nodes[uid].id != uid) {
        std::cout << "Node " << uid << " not exists" << std::endl;
        throw std::invalid_argument("Node not exists");
    }
    return &nodes[uid];
}

std::pair<VArrP, VSetP> BasicTree::find_path_vset(int uid, int vid) {
    reroot(vid);
    auto vec = make_VArr();
    auto path = make_VSet();
    auto u = get_node(uid);
    while (u != nullptr) {
        path->insert(u->id);
        vec->push_back(u->id);
        if (u->id == vid) {
            return {vec, path};
        }
        u = u->parent;
    }
    path->clear();
    vec->clear();
    return {vec, path};
}
