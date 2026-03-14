

#include "../BBF.h"

#include <cassert>
#include <iostream>
#include <queue>
#include <tree.h>

#include "AUF.h"

bool BBF::is_visited(Node *node) const {
    int timestamp = node->timestamp;
    int index = timestamp - t_start_;
    return visited_edge_timestamp_[index] == t_max_;
}

void BBF::mark_visited(Node *node) {
    int timestamp = node->timestamp;
    int index = timestamp - t_start_;
    visited_edge_timestamp_[index] = t_max_;
}

void BBF::mark_unvisited(Node *node) {
    int timestamp = node->timestamp;
    int index = timestamp - t_start_;
    visited_edge_timestamp_[index] = -1;
}

BBF::Stat BBF::get_statistics() const {
    int count = 0;
    int height = 0;

    unordered_map<VId, int> h;

    for (Node *r: roots) {
        auto bcc = std::make_shared<BCC>();
        std::queue<Node *> q;
        q.push(r);

        h.insert({r->timestamp, 1});

        while (!q.empty()) {
            Node *node = q.front();
            q.pop();

            const int cur_h = h.at(node->timestamp);
            height = std::max(height, cur_h);

            if (!node->is_deleted && node->timestamp >= t_start_) {
                count++;
                if (!node->children.empty()) {
                    for (auto c: node->children) {
                        q.push(c);
                        h.insert({c->timestamp, cur_h + 1});
                    }
                }
            }
        }
    }

    Stat stat;
    stat.nodes_count = count;
    stat.height = height;
    return stat;
}

BBF::Node *BBF::get_node(int t) const {
    assert(is_register_timestamp(t));
    return edge_to_node_.at(t);
}

bool BBF::is_register_timestamp(int t) const {
    return edge_to_node_.contains(t);
}

void BBF::register_node(Node *n) {
    assert(!is_register_timestamp(n->timestamp));
    edge_to_node_.insert({n->timestamp, n});
}

void BBF::unregister_node(Node *y) {
    assert(is_register_timestamp(y->timestamp));
    y->is_deleted = true;
#ifndef NDEBUG
    auto yy = get_node(y->timestamp);
    assert(yy == y);
#endif
    edge_to_node_.erase(y->timestamp);
}

BBF::~BBF() {
    while (!tree_node_queue_.empty()) {
        Node *node = tree_node_queue_.front();
        tree_node_queue_.pop_front();
        del_node(node);
    }
}


BBF::Node *BBF::make_new_node(const Edge &edge, int timestamp) {
    t_max_ = std::max(t_max_, timestamp);

    Node *new_node = nullptr;
    if (use_pool_) {
        new_node = node_pool_.getNode(timestamp);
        new_node->gamma = edge;
    } else {
        new_node = new Node{edge, timestamp};
    }


    tree_node_queue_.push_back(new_node);

    register_node(new_node);

    roots.insert(new_node);

    return new_node;
}

void BBF::del_node(Node *n) {
    if (!use_pool_) {
        delete n;
    }
    }


void BBF::insert_edge_basic(const Edge &new_edge, int timestamp, const TEdge &victim_edge,
                            const std::vector<TEdge> &path) {
    make_new_node(new_edge, timestamp);

    if (path.empty()) {
        return;
    }

    std::vector<Node *> multi_parent_nodes;
    build_BEG(victim_edge, path, multi_parent_nodes);

    while (!multi_parent_nodes.empty()) {
        Node *x = multi_parent_nodes.back();

        Node *z = swing(x);

        if (x->out_nbrs.empty()) {
            multi_parent_nodes.pop_back();
            mark_unvisited(x);
        }

        if (z != nullptr && !z->out_nbrs.empty()) {
            if (!is_visited(z)) {
                mark_visited(z);
                multi_parent_nodes.push_back(z);
            }
        }

        if (z != nullptr && z->timestamp != victim_edge.second && z->cache_validity == t_max_) {
            Node *p = z;
            while (p->primary_parent != nullptr && p->primary_parent->out_nbrs.empty()) {
                if (p->cache_validity == t_max_) {
                    break;
                }
                if (p->primary_parent->primary_parent == nullptr) {
                    p->cache_validity = t_max_;
                    p->primary_parent->cache_validity = t_max_;
                    break;
                }
                p = p->primary_parent;
            }
            p->cache_validity = t_max_;
        }
    }
}

int BBF::get_bbf_height() const {
    return get_statistics().height;
}

void BBF::get_bcc_in_subtree_and_update_cache_and_update_roots(Node *r, std::vector<int> &acc) {
    if (r->cache_validity == 0) {
        auto &cache = r->cache_bcc;
        acc.reserve(cache.size());
        for (auto c: cache) {
            acc.push_back(c);
        }
    } else if (!r->children.empty()) {
        acc.reserve(8 * r->children.size());

        for (auto child: r->children) {
            assert(!child->is_deleted);

            auto &cache = child->cache_bcc;
            if (child->cache_validity != 0) {
                get_bcc_in_subtree_and_mark_root(child, cache);
                child->cache_validity = 0;
            } else {
                }
            for (auto c: cache) {
                acc.push_back(c);
            }
        }
    } else {
        acc.reserve(2);
        acc.push_back(r->gamma.first);
        acc.push_back(r->gamma.second);
    }
}

void BBF::get_bcc_in_subtree_and_update_cache(Node *r, std::vector<int> &acc) {
    if (r->cache_validity == 0) {
        auto &cache = r->cache_bcc;
        acc.reserve(cache.size());
        for (auto c: cache) {
            acc.push_back(c);
        }
    } else if (!r->children.empty()) {
        acc.reserve(8 * r->children.size());

        for (auto child: r->children) {
            assert(!child->is_deleted);

            auto &cache = child->cache_bcc;
            if (child->cache_validity != 0) {
                get_bcc_in_subtree(child, cache);
                child->cache_validity = 0;
            } else {
                }
            for (auto c: cache) {
                acc.push_back(c);
            }
        }
    } else {
        acc.reserve(2);
        acc.push_back(r->gamma.first);
        acc.push_back(r->gamma.second);
    }
}

void BBF::get_bccs_cache_trick(vector<vector<VId> > &bccs) {

    int i = 0;

    assert(roots.size() == roots_vec_.size());

    if (roots_vec_timestamp_ == t_max_) {
        for (auto r: roots_vec_) {
            assert(!r->is_deleted);

            std::vector<int> &acc = bccs[i];
            i++;

            get_bcc_in_subtree_and_update_cache(r, acc);
        }
    } else {
        for (auto r: roots) {
            assert(!r->is_deleted);

            std::vector<int> &acc = bccs[i];
            i++;

            get_bcc_in_subtree_and_update_cache(r, acc);
        }
    }
}


void BBF::get_bcc_in_subtree(BbfNode *r, vector<VId> &bcc) {
    retrival_epoch_id_--;

    auto &q = bfs_deque_;
    assert(q.empty());

    q.push_back(r);
    bcc.clear();

    while (!q.empty()) {
        Node *node = q.front();
        q.pop_front();
        if (node->children.empty()) {
            const int u = node->gamma.first;
            const int v = node->gamma.second;

            if (u < 0 || v < 0) {
                assert(node->is_deleted);
                continue;
            }

            assert(u != v);
            assert(v >= 0);
            assert(u >= 0);
            assert(v < vertex_retrival_mark_.size());
            assert(u < vertex_retrival_mark_.size());


            if (vertex_retrival_mark_[u] != retrival_epoch_id_) {
                vertex_retrival_mark_[u] = retrival_epoch_id_;

                bcc.push_back(u);
            }

            if (vertex_retrival_mark_[v] != retrival_epoch_id_) {
                vertex_retrival_mark_[v] = retrival_epoch_id_;
                bcc.push_back(v);
            }
        } else {
            for (auto c: node->children) {
                assert(!c->is_deleted);
                q.push_back(c);
            }
        }
    }

    assert(bcc.size() != 0);
    assert(bcc.size() >= 2);
}

void BBF::get_bcc_in_subtree_and_mark_root(BbfNode *r, vector<VId> &bcc) {
    retrival_epoch_id_--;

    auto &q = bfs_deque_;
    assert(q.empty());

    q.push_back(r);
    bcc.clear();

    while (!q.empty()) {
        Node *node = q.front();
        q.pop_front();
        if (node->children.empty()) {
            const int u = node->gamma.first;
            const int v = node->gamma.second;

            node_pool_.root_timestamp_validity[node->timestamp] = t_max_;
            node_pool_.root_timestamps[node->timestamp] = r->timestamp;

            if (u < 0 || v < 0) {
                assert(node->is_deleted);
                continue;
            }

            assert(u != v);
            assert(v >= 0);
            assert(u >= 0);
            assert(v < vertex_retrival_mark_.size());
            assert(u < vertex_retrival_mark_.size());


            if (vertex_retrival_mark_[u] != retrival_epoch_id_) {
                vertex_retrival_mark_[u] = retrival_epoch_id_;

                bcc.push_back(u);
            }

            if (vertex_retrival_mark_[v] != retrival_epoch_id_) {
                vertex_retrival_mark_[v] = retrival_epoch_id_;
                bcc.push_back(v);
            }
        } else {
            for (auto c: node->children) {
                assert(!c->is_deleted);
                q.push_back(c);
            }
        }
    }

    assert(bcc.size() != 0);
    assert(bcc.size() >= 2);
}

void BBF::clean_deleted_roots() {
    for (auto it = roots.begin(); it != roots.end();) {
        assert((*it)->is_deleted == false);
        if ((*it)->is_deleted) {
            it = roots.erase(it);
        } else {
            ++it;
        }
    }
}

int BBF::get_root_count() const {
    return roots.size();
}

void BBF::get_bccs_label() {
    for (auto r: roots) {
        if (r->is_deleted) {
            continue;
        }
        get_bccs_in_subtree_label_based(r);
    }
}

void BBF::get_bccs_in_subtree_label_based(BbfNode *node) {
    auto &q = bfs_deque_;
    assert(q.empty());

    if (node->children.empty()) {
        vertex_label_[node->gamma.first] = label_id;
        vertex_label_[node->gamma.second] = label_id;
        label_id++;
        return;
    }

    q.push_back(node);
    while (!q.empty()) {
        Node *node = q.front();
        q.pop_front();
        for (auto c: node->children) {
            if (c->children.empty()) {
                assert(!c->is_deleted);
                vertex_label_[c->gamma.first] = label_id;
                vertex_label_[c->gamma.second] = label_id;
            } else {
                q.push_back(c);
            }
        }
    }

    label_id++;
}

void BBF::fresh_roots_vec() {
    roots_vec_.clear();
    for (auto it = roots.begin(); it != roots.end();) {
        if ((*it)->is_deleted) {
            it = roots.erase(it);
            continue;
        }
        roots_vec_.push_back(*it);
        ++it;
    }
    roots_vec_timestamp_ = t_max_;
}

void BBF::get_bcc_of_tree_edges(std::vector<std::vector<VId> > &bccs, const vector<int> &edge_timestamps) {
    const int label = retrival_epoch_id_--;

    bccs.clear();
    bccs.resize(edge_timestamps.size());

    size_t i = 0;
    for (auto timestamp: edge_timestamps) {
        assert(use_pool_);
        Node *node = node_pool_.getNode(timestamp);

        if (node_pool_.root_timestamp_validity[timestamp] == t_max_) {
            int root_timestamp = node_pool_.root_timestamps[timestamp];
            Node *ancestor = node_pool_.getNode(root_timestamp);
            if (!ancestor->is_deleted) {
                node = ancestor;
            }
        }

        while (node->ss_timestamp != label && node->primary_parent != nullptr) {
            node->ss_timestamp = label;

            node = node->primary_parent;

            if (node->ss_timestamp != label && node->primary_parent == nullptr) {
                node->ss_timestamp = label;
                get_bcc_in_subtree_and_update_cache_and_update_roots(node, bccs[i]);
                i++;
            }
        }

        if (node->ss_timestamp != label && node->primary_parent == nullptr) {
            node->ss_timestamp = label;
            get_bcc_in_subtree_and_update_cache_and_update_roots(node, bccs[i]);
            i++;
        }
    }
}


void BBF::mark_leaves(BBF::Node *last_node) {
    if (!last_node) return;

    Node *root = last_node;
    std::queue<Node *> q;
    q.push(last_node);

    while (!q.empty()) {
        Node *node = q.front();
        q.pop();
        for (Node *child: node->children) {
            if (child) q.push(child);
        }
    }
}

void BBF::insert_edge_adv(const Edge &new_edge, int timestamp, const TEdge &victim_edge,
                          const std::vector<TEdge> &path) {
    make_new_node(new_edge, timestamp);

    if (path.empty()) {
        return;
    }

    std::vector<Node *> multi_parent_nodes;
    build_BEG(victim_edge, path, multi_parent_nodes);
    Node *victim = get_node(victim_edge.second);

    while (!multi_parent_nodes.empty()) {
        Node *x = multi_parent_nodes.back();

        Node *z = swing_adv(x, victim);

        if (x->out_nbrs.empty()) {
            multi_parent_nodes.pop_back();
            x->simple_versatile_mark = false;
        }

        if (z != nullptr && !z->out_nbrs.empty()) {
            if (!z->simple_versatile_mark) {
                z->simple_versatile_mark = true;
                multi_parent_nodes.push_back(z);
            }
        }

        }
}

void BBF::insert_edge_adv_wc(const Edge &new_edge, int timestamp, const TEdge &victim_edge,
                             const std::vector<TEdge> &path) {
    make_new_node(new_edge, timestamp);

    if (path.empty()) {
        return;
    }

    std::vector<Node *> multi_parent_nodes;
    build_BEG(victim_edge, path, multi_parent_nodes);
    Node *victim = get_node(victim_edge.second);

    while (!multi_parent_nodes.empty()) {
        Node *x = multi_parent_nodes.back();

        Node *z = swing_adv(x, victim);

        if (x->out_nbrs.empty()) {
            multi_parent_nodes.pop_back();
            x->simple_versatile_mark = false;
        }

        if (z != nullptr && !z->out_nbrs.empty()) {
            if (!z->simple_versatile_mark) {
                z->simple_versatile_mark = true;
                multi_parent_nodes.push_back(z);
            }
        }

        if (z != nullptr && z->cache_validity == t_max_) {
            Node *p = z;
            while (p->primary_parent != nullptr && p->primary_parent->out_nbrs.empty()) {
                if (p->cache_validity == t_max_) {
                    break;
                }
                if (p->primary_parent->primary_parent == nullptr) {
                    p->cache_validity = t_max_;
                    p->primary_parent->cache_validity = t_max_;
                    break;
                }
                p = p->primary_parent;
            }
            p->cache_validity = t_max_;
        }
    }
}

void BBF::insert_edge_adv_oq(const Edge &new_edge, int timestamp, const TEdge &victim_edge,
                             const std::vector<TEdge> &path) {
    make_new_node(new_edge, timestamp);

    if (path.empty()) {
        return;
    }

    std::vector<Node *> multi_parent_nodes;
    build_BEG(victim_edge, path, multi_parent_nodes);
    Node *victim = get_node(victim_edge.second);

    Node *last_node = nullptr;
    while (!multi_parent_nodes.empty()) {
        Node *x = multi_parent_nodes.back();

        last_node = x;

        Node *z = swing_adv(x, victim);

        if (x->out_nbrs.empty()) {
            multi_parent_nodes.pop_back();
            x->simple_versatile_mark = false;
        }

        if (z != nullptr && !z->out_nbrs.empty()) {
            if (!z->simple_versatile_mark) {
                z->simple_versatile_mark = true;
                multi_parent_nodes.push_back(z);
            }
        }
    }

    if (last_node != nullptr) {
        while (last_node->primary_parent != nullptr) {
            last_node = last_node->primary_parent;
        }

        mark_leaves(last_node);
    }
}


void BBF::expire(int expired_timestamp) {
    std::vector<Edge> tmp;
    expire(expired_timestamp, tmp);
}

void BBF::expire(int expired_timestamp, std::vector<Edge> &expired_tree_edges) {
    expired_tree_edges.clear();

    while (!tree_node_queue_.empty()) {
        Node *node = tree_node_queue_.front();

        if (node->timestamp > expired_timestamp) break;

        t_start_ = node->timestamp;

        if (!node->is_deleted) {
            std::vector<Node *> to_cut(node->children.begin(), node->children.end());
            for (auto c: to_cut) {
                if (!node->is_deleted) {
                    cut(c, node);
                }
            }
        } else {
            assert(node->children.empty());
        }

        roots.erase(node);


        tree_node_queue_.pop_front();

        if (node->gamma.first > 0 || node->gamma.second > 0) {
            expired_tree_edges.push_back(node->gamma);
        }

        if (is_register_timestamp(node->timestamp)) {
            unregister_node(node);
        }

        del_node(node);
        }
}

void BBF::expire_qo(int expired_timestamp, std::vector<Edge> &expired_tree_edges) {
    expired_tree_edges.clear();

    while (!tree_node_queue_.empty()) {
        Node *node = tree_node_queue_.front();

        if (node->timestamp > expired_timestamp) break;
        assert(roots.contains(node));

        t_start_ = node->timestamp;


        std::vector<Node *> to_cut(node->children.begin(), node->children.end());
        for (auto c: to_cut) {
            if (!node->is_deleted) {
                cut(c, node);
            }

            mark_leaves(c);
        }


        tree_node_queue_.pop_front();

        if (node->gamma.first > 0 || node->gamma.second > 0) {
            expired_tree_edges.push_back(node->gamma);
        }


        if (is_register_timestamp(node->timestamp)) {
            unregister_node(node);
        }

        roots.erase(node);

        del_node(node);
        }
}

void BBF::get_bccs(vector<vector<VId> > &bccs) {


    int i = 0;

    if (roots_vec_timestamp_ == t_max_) {
        for (auto r: roots_vec_) {
            if (r->is_deleted) {
                continue;
            }

            std::vector<int> &acc = bccs.at(i);
            i++;
            acc.reserve(16);

            get_bcc_in_subtree(r, acc);
        }
    } else {
        for (auto r: roots) {
            if (r->is_deleted) {
                continue;
            }

            std::vector<int> &acc = bccs[i];
            i++;
            acc.reserve(16);

            get_bcc_in_subtree(r, acc);
        }
    }
}

void BBF::get_bccs(BCCs &bccs) {
    bccs.clear();
    bccs.reserve(roots.size());

    vector<vector<VId> > vbccs;
    vbccs.resize(roots.size());
    get_bccs(vbccs);


    for (auto b: vbccs) {
        if (b.empty()) {
            continue;
        }

        auto bcc = std::make_shared<BCC>();
        bcc->insert(b.begin(), b.end());
        assert(bcc->size() >= 2);

        bccs.emplace_back(std::move(bcc));
    }
}

bool BBF::has_lca(int timestamp_v, int timestamp_w) {
    assert(is_register_timestamp(timestamp_v));
    assert(is_register_timestamp(timestamp_w));

    Node *v = get_node(timestamp_v);
    Node *w = get_node(timestamp_w);

    while (v && w && v->timestamp != w->timestamp) {
        if (v->timestamp > w->timestamp) {
            v = v->primary_parent;
        } else {
            w = w->primary_parent;
        }
    }
    return (v && v == w);
}

bool BBF::has_lca_qo(int timestamp_v, int timestamp_w) {
    assert(is_register_timestamp(timestamp_v));
    assert(is_register_timestamp(timestamp_w));

    Node *v = get_node(timestamp_v);
    Node *w = get_node(timestamp_w);

    return false;
}

void BBF::add_isolated_node(const Edge &eg, int timestamp) {
    make_new_node(eg, timestamp);
}

BBF::Node *BBF::get_a_parent_other_than_y(BBF::Node *x, BBF::Node *y) {
    if (x->primary_parent != nullptr && x->primary_parent->timestamp > y->timestamp) {
        return x->primary_parent;
    }
    for (auto z: x->out_nbrs) {
        if (z->timestamp > y->timestamp) {
            return z;
        }
    }
    return nullptr;
}


void BBF::bottom_up_maintenance(Time_Max_Queue &Q) {
    AUF auf(t_start_, t_max_);

    std::vector<Node *> to_delete;
    while (!Q.empty()) {
        Node *y = Q.top();
        Q.pop();

        assert(is_visited(y));
        assert(!y->is_deleted);

        if (!auf.is_in(y)) {
            auf.make_set(y);
        }

        for (auto p: y->out_nbrs) {
            if (!is_visited(p)) {
                mark_visited(p);
                Q.push(p);
            }
        }
        if (y->primary_parent != nullptr) {
            auto p = y->primary_parent;
            if (!is_visited(p)) {
                mark_visited(p);
                Q.push(p);
            }
        }

        const std::vector<Node *> children(y->children.begin(), y->children.end());
        assert(children.size() == y->children.size());
        for (Node *x: children) {
            if (!auf.is_in(x)) {
                auf.make_set(x);
            }

            Node *z = get_a_parent_other_than_y(x, y);

            Node *c = nullptr;
            if (z != nullptr) {
                auto z_top = auf.find(z);
                auto low = auf.get_anchor(z_top);
                assert(!low->is_deleted);

                if (low != y) {
                    assert(low->timestamp > y->timestamp);


                    if (low->children.size() == 1 && low->primary_parent == nullptr && low->out_nbrs.empty()) {

                        c = *low->children.begin();
                        assert(c != low);
                        assert(!c->is_deleted);

                        link(c, y);
                        cut(c, low);

                        assert(roots.contains(low));
                        unregister_node(low);
                        roots.erase(low);
                        assert(!roots.contains(c));
                    } else {
                        link(low, y);
                    }
                }

                assert(x->primary_parent == y || x->out_nbrs.contains(y));
                if (c != x) {
                    cut(x, y);
                }
            }

            auf.union_sets(x, y);
            assert(auf.get_anchor(x) == y);
        }

        if (y->children.size() == 1) {
            to_delete.push_back(y);
        }
    }

    for (auto x: to_delete) {
        assert(x->children.size() <= 1);
        if (x->is_deleted) {
            continue;
        }
        auto p = x->primary_parent;
        assert(x->out_nbrs.empty());
        assert(x->children.size() == 1);
        auto c = *(x->children.begin());
        unregister_node(x);
        if (p != nullptr) {
            link(c, p);
            cut(x, p);
        } else {
            assert(roots.contains(x));
            roots.erase(x);
        }
        cut(c, x);
        assert(!roots.contains(x));
    }
}

bool BBF::is_tree_and_no_deleted_root() const {
    for (int i = 0; i < tree_node_queue_.size(); i++) {
        auto node = tree_node_queue_.at(i);
        assert(node->primary_parent == nullptr || (node->primary_parent != nullptr && node->out_nbrs.empty()));
    }

    for (auto r: roots) {
        assert(!r->is_deleted);
        assert(r->timestamp >= t_max_ - THETA_ + 1);
        assert(r->timestamp <= t_max_);
    }


    return true;
}

bool BBF::is_no_single_child_and_no_deleted_child() const {
    for (int i = 0; i < tree_node_queue_.size(); i++) {
        auto node = tree_node_queue_.at(i);
        const auto &children = node->children;
        assert(children.empty() || children.size() > 1);
        if (!node->is_deleted) {
            for (auto c: children) {
                assert(!c->is_deleted);
                assert(c->timestamp >= t_max_ - THETA_ + 1);
                assert(c->timestamp <= t_max_);
            }
        } else {
            assert(children.size() == 0);
        }
    }
    return true;
}

int BBF::count_nodes() const {
    return get_statistics().nodes_count;
}

double BBF::get_index_size_in_KB() {
    size_t storage_count = roots.size();
    storage_count += tree_node_queue_.size();

    for (auto r: roots) {
        std::queue<Node *> q;
        q.push(r);
        while (!q.empty()) {
            Node *node = q.front();
            q.pop();

            if (!node->is_deleted && node->timestamp >= t_start_) {
                if (node->children.empty()) {
                    storage_count += 2;
                }
                storage_count += 4;
                storage_count += node->children.size();

                if (!node->children.empty()) {
                    for (auto c: node->children) {
                        q.push(c);
                    }
                }
            }
        }
    }

    return (int) storage_count * 8 / 1024.0;
}


void BBF::build_BEG(const TEdge &victim, const std::vector<TEdge> &path, Time_Max_Queue &Q) {
    Node *victim_node = get_node(victim.second);
    mark_intermediate_node(victim_node);

    for (int i = 0; i < path.size(); i++) {
        const TEdge &edge = path.at(i);
        Node *node = get_node(edge.second);
        assert(node->timestamp > victim_node->timestamp);

        link(node, victim_node);

        auto p = node->primary_parent;
        if (!is_visited(p)) {
            mark_visited(p);
            Q.push(p);
        }
        for (auto op: node->out_nbrs) {
            if (!is_visited(op)) {
                mark_visited(op);
                Q.push(op);
            }
        }
    }
}

void BBF::mark_intermediate_node(BBF::Node *victim_node) {
    if (victim_node->gamma.first > 0 || victim_node->gamma.second > 0) {
        victim_node->gamma.first = -victim_node->gamma.first;
        victim_node->gamma.second = -victim_node->gamma.second;
    }
}


void BBF::build_BEG(const TEdge &victim, const std::vector<TEdge> &edges, std::vector<Node *> &multi_parent_nodes) {
    Node *victim_node = get_node(victim.second);
    mark_intermediate_node(victim_node);

    for (int i = 0; i < edges.size(); i++) {
        const TEdge &edge = edges.at(i);
        Node *node = get_node(edge.second);
        assert(node->timestamp > victim_node->timestamp);
        link(node, victim_node);
        if (!node->out_nbrs.empty()) {
            multi_parent_nodes.push_back(node);
        }
    }
}


BBF::Node *BBF::swing(Node *x) {
    assert(!x->out_nbrs.empty());

    Node *y = x->primary_parent;
    Node *z = *(x->out_nbrs.begin());


    if (y->timestamp > z->timestamp) {
        std::swap(y, z);
    }

    if (z->primary_parent == nullptr && z->out_nbrs.empty()) {
        z->cache_validity = t_max_;
    }

    link(z, y);
    recursive_cut(x, y);

    return z;
}

bool BBF::is_ancestor(Node *descendant, Node *ancestor) {
    while (descendant != nullptr && ancestor != descendant) {
        descendant = descendant->primary_parent;
    }
    return descendant == ancestor & ancestor != nullptr;
}

BBF::Node *BBF::swing_adv(Node *x, Node *victim_node) {
    assert(!x->out_nbrs.empty());

    Node *y = x->primary_parent;
    Node *z = *(x->out_nbrs.begin());

    if (y->timestamp > z->timestamp) {
        std::swap(y, z);
    }

    Node *p = z->primary_parent;
    assert(y->out_nbrs.empty());

    while (p != nullptr && p->timestamp > y->timestamp) {
        if (p->ss_timestamp == t_max_ && p->ss_ancestor_timestamp == y->timestamp) {
            assert(is_ancestor(p, y));

            recursive_cut(x, y);

            return nullptr;
        }

        p->ss_timestamp = t_max_;
        p->ss_ancestor_timestamp = y->timestamp;

        z = p;
        p = p->primary_parent;
    }

    if (z != victim_node && z->primary_parent == nullptr && z->out_nbrs.empty()) {
        z->cache_validity = t_max_;
    }

    link(z, y);
    recursive_cut(x, y);

    z->ss_timestamp = t_max_;
    z->ss_ancestor_timestamp = y->timestamp;

    return z;
}

void BBF::recursive_cut(Node *x, Node *y) {
    assert(x->primary_parent == y || x->out_nbrs.contains(y));

    cut(x, y);
    assert(!y->children.contains(x));

    if (y->children.size() == 1) {
        unregister_node(y);

        auto p = y->primary_parent;
        assert(y->out_nbrs.empty());
        auto c = *y->children.begin();
        if (p != nullptr) {
            link(c, p);
            cut(y, p);
            assert(!p->children.contains(y));
        } else {
            assert(roots.contains(y));
            roots.erase(y);
        }

        assert(y->is_deleted);
        assert(!roots.contains(y));


        cut(c, y);
        assert(!y->children.contains(c));
    }
}

void BBF::cut(Node *x, Node *y) {
    if (x->primary_parent == y) {
        if (!x->out_nbrs.empty()) {
            Node *z = *(x->out_nbrs.begin());
            assert(z->timestamp < x->timestamp);
            x->primary_parent = z;
            x->out_nbrs.erase(z);
        } else {
            x->primary_parent = nullptr;
            if (!x->is_deleted) {
                roots.insert(x);
            }
        }
    } else if (x->out_nbrs.contains(y)) {
        assert(x->primary_parent != nullptr);
        x->out_nbrs.erase(y);
    }

    #ifdef USING_SET_FOR_CHILDREN
    assert(y->children.contains(x));
    y->children.erase(x);
#else
    assert(find(y->children.begin(), y->children.end(), x) != y->children.end());
    std::vector<Node *> &children = y->children;
    auto it = std::find(children.begin(), children.end(), x);
    if (it != children.end()) {
        *it = children.back();
        children.pop_back();
    }
#endif
}

void BBF::link(Node *x, Node *y) {
    assert(x->timestamp > y->timestamp);
    assert(!x->is_deleted);
    assert(!y->is_deleted);

    if (x->primary_parent == nullptr) {
        assert(roots.contains(x));
        roots.erase(x);
    }

    if (x->primary_parent == nullptr) {
        x->primary_parent = y;
    } else if (x->primary_parent != y) {
        x->out_nbrs.insert(y);
    }

#ifdef USING_SET_FOR_CHILDREN
    y->children.insert(x);
#else
    if (find(y->children.begin(), y->children.end(), x) == y->children.end()) {
        y->children.push_back(x);
    }
#endif
}