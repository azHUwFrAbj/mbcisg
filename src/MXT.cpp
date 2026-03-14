

#include "MXT.h"

#include <cassert>
#include <iostream>
#include <vector>


MXT::MXT(int N) : N_(N + 1) {
    for (int i = 0; i < N_; ++i) {
        vid2_nodes_[i] = new Node();
        vid2_nodes_[i]->vid_ = i;
    }
    path.reserve(N_ + 1);
    weights.reserve(N_ + 1);
}

MXT::~MXT() {
    for (int i = 0; i < N_; ++i) {
        delete vid2_nodes_[i];
    }
}

TEdge MXT::insert_edge_and_get_t_path(const Edge &eg, const int timestamp, std::vector<TEdge> &path) {
    const int vid = eg.first;
    const int wid = eg.second;

    Edge victim;
    path.clear();

    auto v = vid2_nodes_.at(vid);
    auto w = vid2_nodes_.at(wid);

    if (w->parent_ == v) {
        std::swap(v, w);
    }
    if (v->parent_ == w) {
        int old_time = v->timestamp_;
        v->timestamp_ = timestamp;
        return {victim, old_time};
    }

    int min_timestamp = timestamp;
    int victim_index = -1;

    while (v != nullptr && w != nullptr && v != w) {
        if (v->size_ > w->size_) {
            std::swap(v, w);
        }

        if (v->parent_ != nullptr) {
            auto eg = make_edge(v->vid_, v->parent_->vid_);
            if (v->timestamp_ < min_timestamp) {
                victim = eg;
                victim_index = (int) path.size();
                min_timestamp = v->timestamp_;
            }
            path.push_back({eg, v->timestamp_});
        }
        v = v->parent_;
    }

    if (v == w) {
        auto new_eg = make_edge(vid, wid);
        swap_in(victim, new_eg, timestamp);
        path.at(victim_index) = {new_eg, timestamp};
    } else {
        link(vid, wid, timestamp);
        path.clear();
    }

    return {victim, min_timestamp};
}

void dfs_marking(MXT::Node *node, int &timestamp, MXT::Node *root) {
    if (node == nullptr) {
        return;
    }

    node->left_ = timestamp;
    node->root_ = root;
    timestamp += 1;

    for (auto nb: node->nbr_) {
        if (nb != node->parent_) {
            dfs_marking(nb, timestamp, root);
        }
    }

    node->right_ = timestamp;
}

void MXT::dfs_mark_parent(const Edge &edge) {
    auto x = vid2_nodes_.at(edge.first);
    auto y = vid2_nodes_.at(edge.second);

    Node *root = x->parent_ == y ? y : x;
    assert(root != nullptr && root->parent_ == nullptr);

    int timestamp = 0;
    dfs_marking(root, timestamp, root);
}

void MXT::dfs_mark_child(const Edge &edge) {
    auto x = vid2_nodes_.at(edge.first);
    auto y = vid2_nodes_.at(edge.second);

    Node *root = x->parent_ == y ? x : y;
    assert(root->parent_ == x || root->parent_ == y);

    int timestamp = 0;
    dfs_marking(root, timestamp, root);
}

double MXT::get_index_size_in_KB() {
    int storage_count = 0;
    for (auto vid_node_pair: vid2_nodes_) {
        Node *node = vid_node_pair.second;
        if (node == nullptr || node->nbr_.empty()) {
            continue;
        }

        storage_count += 2;
        storage_count += (int) node->nbr_.size();
    }

    return storage_count * 8 / 1024.0;
}

std::pair<Edge, Edge> MXT::find_path_edge(VId vid, VId wid) const {
    Edge v_edge = make_edge(-1, -1);
    Edge w_edge = make_edge(-1, -1);


    auto x = vid2_nodes_.at(vid);
    auto y = vid2_nodes_.at(wid);

    if (x->size_ > y->size_) {
        std::swap(x, y);
    }

    if (x->parent_ == nullptr) {
        return std::make_pair(make_edge(-1, -1), make_edge(-1, -1));
    }

    VId z = -1;
    Edge z_edge;
    bool find_z = false;

    if (x->vid_ == vid) {
        z = wid;
        v_edge = make_edge(x->vid_, x->parent_->vid_);
    } else {
        z = vid;
        w_edge = make_edge(x->vid_, x->parent_->vid_);
    }

    while (x != nullptr && y != nullptr && x != y) {
        if (x->size_ > y->size_) {
            std::swap(x, y);
        }

        if (x->vid_ == z
            || (x->parent_ != nullptr && x->parent_->vid_ == z)
        ) {
            if (x->parent_ != nullptr) {
                z_edge = make_edge(x->vid_, x->parent_->vid_);
                find_z = true;
            }
            break;
        }

        x = x->parent_;
    }

    if (find_z) {
        if (z == vid) {
            return std::make_pair(z_edge, w_edge);
        }
        return std::make_pair(v_edge, z_edge);
    }

    return std::make_pair(make_edge(-1, -1), make_edge(-1, -1));
}

bool is_ancestor(MXT::Node *x, MXT::Node *y) {
    return x->left_ <= y->left_ && x->right_ >= y->right_;
}

std::pair<int, int> MXT::find_path_edge_timestamp_qo(VId vid, VId wid) const {
    auto x = vid2_nodes_.at(vid);
    auto y = vid2_nodes_.at(wid);

    if (x->root_ != y->root_) {
        return std::make_pair(-1, -1);
    }


    if (is_ancestor(x, y)) {
        auto p = y;
        while (p->parent_ != x && p != nullptr) {
            p = p->parent_;
        }
        if (p->parent_ == x) {
            return std::make_pair(p->timestamp_, y->timestamp_);
        }
        assert(false && "cannot reach here");
    }

    if (is_ancestor(y, x)) {
        auto p = x;
        while (p->parent_ != y && p != nullptr) {
            p = p->parent_;
        }
        if (p->parent_ == y) {
            return std::make_pair(p->timestamp_, x->timestamp_);
        }
        assert(false && "cannot reach here");
    }
    return std::make_pair(x->timestamp_, y->timestamp_);
}

std::pair<int, int> MXT::find_path_edge_timestamp(VId vid, VId wid) const {
    auto x = vid2_nodes_.at(vid);
    auto y = vid2_nodes_.at(wid);

    if (x->size_ > y->size_) {
        std::swap(x, y);
    }

    if (x->parent_ == nullptr) {
        return std::make_pair(-1, -1);
    }

    VId z = -1;
    int z_timestamp = -1;
    int x_timestamp = x->timestamp_;
    bool find_z = false;

    if (x->vid_ == vid) {
        z = wid;
    } else {
        z = vid;
    }

    while (x != nullptr && y != nullptr && x != y) {
        if (x->size_ > y->size_) {
            std::swap(x, y);
        }

        if (x->vid_ == z
            || (x->parent_ != nullptr && x->parent_->vid_ == z)
        ) {
            if (x->parent_ != nullptr) {
                z_timestamp = x->timestamp_;
                find_z = true;
            }
            break;
        }

        x = x->parent_;
    }

    if (find_z) {
        return std::make_pair(x_timestamp, z_timestamp);
    }

    return std::make_pair(-1, -1);
}

void MXT::swap_in(const Edge &old_edge, const Edge &new_edge, int timestamp) {
    unlink(old_edge.first, old_edge.second);
    make_root(new_edge.first);
    make_root(new_edge.second);

    auto x = vid2_nodes_.at(new_edge.first);
    auto y = vid2_nodes_.at(new_edge.second);

    if (x->size_ > y->size_) {
        std::swap(x, y);
    }

    link_roots(x->vid_, y->vid_, timestamp);
}

TEdge MXT::insert_edge(VId vid, VId wid, int timestamp, std::vector<Edge> &path) {
    path.clear();

    Edge victim;
    int min_timestamp = timestamp;
    int victim_index = -1;

    auto v = vid2_nodes_.at(vid);
    auto w = vid2_nodes_.at(wid);

    while (v != w && v != nullptr && w != nullptr) {
        if (v->size_ > w->size_) {
            std::swap(v, w);
        }

        if (v->parent_ != nullptr) {
            auto eg = make_edge(v->vid_, v->parent_->vid_);
            if (v->timestamp_ < min_timestamp) {
                victim = eg;
                victim_index = (int) path.size();
                min_timestamp = v->timestamp_;
            }
            path.push_back(eg);
        }
        v = v->parent_;
    }

    if (v == w) {
        auto new_eg = make_edge(vid, wid);
        swap_in(victim, new_eg, timestamp);
        path.at(victim_index) = new_eg;
    } else {
        link(vid, wid, timestamp);
        path.clear();
    }

    return {victim, min_timestamp};
}

void MXT::direct_delete(VId uid, VId vid) {
    unlink(uid, vid);
}

bool MXT::is_nbr(VId uid, VId vid) const {
    return vid2_nodes_.at(uid)->nbr_.contains(vid2_nodes_.at(vid));
}

void MXT::get_nbr_timestamps(VId uid, std::vector<VId> &nbr) const {
    nbr.clear();
    auto u = vid2_nodes_.at(uid);
    for (auto v: u->nbr_) {
        if (u->parent_ == u) {
            nbr.push_back(v->timestamp_);
        } else {
            nbr.push_back(u->timestamp_);
        }
    }
}

void MXT::link_roots(VId uid, VId vid, int weight) {
    auto u = vid2_nodes_.at(uid);
    auto v = vid2_nodes_.at(vid);

    v->nbr_.insert(u);
    u->nbr_.insert(v);

    u->parent_ = v;
    u->timestamp_ = weight;
    v->size_ = v->size_ + u->size_;
}

void MXT::link(VId uid, VId vid, int weight) {
    make_root(uid);
    make_root(vid);
    link_roots(uid, vid, weight);
}

void MXT::unlink(VId uid, VId vid) {
    auto u = vid2_nodes_.at(uid);
    auto v = vid2_nodes_.at(vid);

    if (v->parent_ == u) {
        std::swap(u, v);
    } else if (u->parent_ != v) {
        return;
    }

    if (v->size_ < u->size_) {
        std::cout << "here" << std::endl;
    }

    u->parent_ = nullptr;
    u->nbr_.erase(v);
    v->nbr_.erase(u);

    for (auto w = v; w != nullptr; w = w->parent_) {
        if (w->size_ < u->size_) {
            std::cout << "here" << std::endl;
        }
        w->size_ = w->size_ - u->size_;
        assert(w->size_ > 0);
    }
}

void MXT::make_root(VId uid) {
    auto u = vid2_nodes_.at(uid);
    make_root(u);
}

void MXT::make_root(Node *u) {
    Node *cur = u;
    while (cur->parent_ != nullptr) {
        path.push_back(cur);
        weights.push_back(cur->timestamp_);
        cur = cur->parent_;
    }

    for (int i = (int) path.size() - 1; i >= 0; --i) {
        Node *child = path[i];
        Node *parent = child->parent_;
        int u_weight = weights[i];

        parent->parent_ = child;
        parent->timestamp_ = u_weight;

        child->parent_ = nullptr;
        child->timestamp_ = -1;

        parent->size_ -= child->size_;
        assert(parent->size_ > 0);
        child->size_ += parent->size_;
    }

    path.clear();
    weights.clear();
}