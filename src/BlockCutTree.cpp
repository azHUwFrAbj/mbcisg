

#include "../BlockCutTree.h"

#include <cassert>
#include "BlockCutTreeNode.h"

BlockCutTree::BlockCutTree(int N) : N_(N + 1) {
    square_nodes_.resize(N_ + 1);
    round_nodes_.resize(N_ + 1);
    free_round_nodes_.resize(N_ + 1);
    for (int i = 0; i <= N_; ++i) {
        auto bc_node = BlockCutTreeNode::new_square_node(i);
        square_nodes_.at(i) = bc_node;

        auto r_node = BlockCutTreeNode::new_round_node(i);
        round_nodes_.at(i) = r_node;
        free_round_nodes_.at(i) = r_node;
    }
}

BlockCutTree::~BlockCutTree() {
    for (int i = 0; i <= N_; ++i) {
        delete square_nodes_.at(i);
    }
    for (auto node: round_nodes_) {
        delete node;
    }
    round_nodes_.clear();
}

void BlockCutTree::build_by_bccs(const std::vector<std::shared_ptr<std::set<int> > > &bccs) {
    for (const auto &bcc: bccs) {
        auto round_node = new_round_node();
        for (auto vertex: *bcc) {
            auto square_node = square_nodes_.at(vertex);
            link(square_node, round_node);
        }
    }
}

void BlockCutTree::build() {
    int max_id = get_max_vid(edges_);
    HopcroftTarjan ht(max_id);
    for (const auto &edge: edges_) {
        ht.add_edge(edge.first, edge.second);
    }
    ht.tarjan_bcc();

#ifndef NDEBUG
    clean();
    const auto &bccs = ht.biconnectedComponents;
    build_by_bccs(bccs);
#endif
}

void BlockCutTree::build(const std::set<VId> &affected_vertices, const std::vector<Edge> &edges_to_delete) {
    HopcroftTarjan ht(N_);

    std::set<BlockCutTreeNode *> affected_bccs;

    for (const auto &edge: edges_) {
        if (affected_vertices.count(edge.first) != 0 && affected_vertices.count(edge.second) != 0) {
            ht.add_edge(edge.first, edge.second);

            auto bcc = get_bcc_label(edge);
            if (bcc != nullptr) {
                affected_bccs.insert(bcc);
            }
        }
    }
    ht.tarjan_bcc();

    for (auto eg: edges_to_delete) {
        auto bcc = get_bcc_label(eg);
        if (bcc != nullptr) {
            affected_bccs.insert(bcc);
        }
    }
    clean(affected_bccs);

    const auto &bccs = ht.biconnectedComponents;
    build_by_bccs(bccs);
}

bool BlockCutTree::bcc_query(int vid, int wid) const {
    BlockCutTreeNode *v = square_nodes_.at(vid);
    BlockCutTreeNode *w = square_nodes_.at(wid);

    if (v->parent_ == w->parent_ || w->get_children().contains(v->parent_->id_)) {
        return true;
    }

    for (const auto &nv: v->get_children()) {
        if (w->get_children().contains(nv) || nv == w->parent_->id_) {
            return true;
        }
    }
    return false;
}

void BlockCutTree::get_bcc(Edge eg, std::set<VId> &bcc) const {
    BlockCutTreeNode *v = square_nodes_.at(eg.first);
    BlockCutTreeNode *w = square_nodes_.at(eg.second);

    bcc.clear();

    auto label = get_bcc_label(eg);
    bcc.insert(label->get_children().begin(), label->get_children().end());
    if (label->parent_ != nullptr) {
        bcc.insert(label->parent_->id_);
    }
}

BlockCutTreeNode *BlockCutTree::get_bcc_label(Edge eg) const {
    BlockCutTreeNode *v = square_nodes_.at(eg.first);
    BlockCutTreeNode *w = square_nodes_.at(eg.second);

    if (v->parent_ == w->parent_ || w->get_children().contains(v->parent_->id_)) {
        return v->parent_;
    }

    for (auto nv: v->get_children()) {
        if (w->parent_->id_ == nv || w->get_children().contains(nv)) {
            return round_nodes_.at(nv);
        }
    }

    return nullptr;
}

void BlockCutTree::run_ht_algorithm(std::vector<std::vector<VId> > &bbcs) {
    int max_id = get_max_vid(edges_);
    HopcroftTarjan ht(max_id);
    for (const auto &edge: edges_) {
        ht.add_edge(edge.first, edge.second);
    }
    ht.run_and_get_bccs(bbcs);
}

void BlockCutTree::run_ht_algorithm_label_based() {
    int max_id = get_max_vid(edges_);
    HopcroftTarjan ht(max_id);
    for (const auto &edge: edges_) {
        ht.add_edge(edge.first, edge.second);
    }
    ht.run_label_based();
}

void BlockCutTree::just_add_edge(int vid, int wid) {
    edges_.push_back(std::make_pair(vid, wid));
}

Edge BlockCutTree::front_edge() const {
    return edges_.front();
}

Edge BlockCutTree::pop_front_edge() {
    Edge eg = edges_.front();
    edges_.pop_front();
    return eg;
}

void BlockCutTree::insert_update(int vid, int wid) {
    just_add_edge(vid, wid);
    auto [path, estimated_size] = find_path(vid, wid);
    BlockCutTreeNode *p_v = nullptr;
    if (path.size() > 1) {
        p_v = path.at(path.size() - 2);
    }

    auto v = square_nodes_.at(vid);
    auto w = square_nodes_.at(wid);

    if (path.empty()) {
        evert(v);
        evert(w);
        auto r_node = new_round_node();
        link(v, r_node);
        link(w, r_node);
    } else {
        evert(v);

        auto r_node = new_round_node();
        r_node->get_children().reserve(estimated_size);

        r_node->children_buffer_.reserve(estimated_size);

        assert(p_v->parent_ == v);
        assert(v->childrens_.contains(p_v->id_));
        v->childrens_.erase(p_v->id_);
        p_v->parent_ = nullptr;

        for (size_t i = 0; i < path.size() - 1; ++i) {
            auto u = path.at(i);
            if (u->is_square()) {
                continue;
            }

            for (auto u_nb_id: u->get_children()) {
                auto u_nb = square_nodes_.at(u_nb_id);
                u_nb->parent_ = r_node;

                r_node->children_buffer_.push_back(u_nb_id);
            }

            if (u->parent_ != nullptr) {
                auto p = u->parent_;
                p->get_children().erase(u->id_);
                p->parent_ = r_node;
            }
            delete_round_node(u);
        }

        link(v, r_node);
    }
}


void BlockCutTree::clean() {
    for (auto square_node: square_nodes_) {
        square_node->childrens_.clear();
        square_node->parent_ = nullptr;
    }

    free_round_nodes_.clear();

    for (auto round_node: round_nodes_) {
        delete_round_node(round_node);
    }
}

void BlockCutTree::clean(const std::set<BlockCutTreeNode *> &affected_round_nodes) {
    for (auto r_node: affected_round_nodes) {
        assert(!r_node->is_square());

        for (auto square_node_id: r_node->get_children()) {
            auto square_node = square_nodes_.at(square_node_id);
            square_node->parent_ = nullptr;
        }
        if (r_node->parent_ != nullptr) {
            auto p = r_node->parent_;
            p->get_children().erase(r_node->id_);
        }
        delete_round_node(r_node);
    }
}

void BlockCutTree::delete_round_node(BlockCutTreeNode *node) {
    assert(!node->is_square());
    free_round_nodes_.push_back(node);
    node->parent_ = nullptr;
    node->children_buffer_.clear();
    node->get_children().clear();
}

bool BlockCutTree::cc_query(int vid, int wid) const {
    auto v = square_nodes_.at(vid);
    auto w = square_nodes_.at(wid);

    evert(v);

    while (w != nullptr && w != v) {
        w = w->parent_;
    }
    return w == v;
}

std::set<int> BlockCutTree::get_cc(int vid) const {
    auto v = square_nodes_.at(vid);
    std::set<int> cc;

    std::vector<BlockCutTreeNode *> stack;
    std::set<BlockCutTreeNode *> visited;
    stack.push_back(v);
    while (!stack.empty()) {
        BlockCutTreeNode *node = stack.back();
        stack.pop_back();
        if (visited.contains(node)) {
            continue;
        }
        if (node->is_square()) {
            cc.insert(node->id_);
        }
        visited.insert(node);
        for (auto nb_id: node->get_children()) {
            BlockCutTreeNode *nb = node->is_square() ? round_nodes_.at(nb_id) : square_nodes_.at(nb_id);
            stack.push_back(nb);
        }
        if (node->parent_ != nullptr) {
            auto pid = node->parent_->id_;
            BlockCutTreeNode *nb = node->is_square() ? round_nodes_.at(pid) : square_nodes_.at(pid);
            stack.push_back(nb);
        }
    }
    return cc;
}

std::pair<PVec<BlockCutTreeNode *>, int> BlockCutTree::find_path(int vid, int wid) const {
    PVec<BlockCutTreeNode *> result;
    int square_node_counts = 0;

    auto v = square_nodes_.at(vid);
    auto w = square_nodes_.at(wid);

    evert(v);
    while (w != nullptr && w != v) {
        square_node_counts += (int) w->childrens_.size() + 1;
        result.push_back(w);
        w = w->parent_;
    }

    if (w == nullptr) {
        result.clear();
        square_node_counts = 0;
    } else {
        square_node_counts += (int) w->childrens_.size() + 1;
        result.push_back(w);
    }
    return {result, square_node_counts};
}

bool BlockCutTree::has_edge() const {
    return !edges_.empty();
}

void BlockCutTree::get_bccs(BCCs &bccs) const {
    bccs.clear();
    for (BlockCutTreeNode *r_node: round_nodes_) {
        if (r_node->get_children().empty() && r_node->parent_ == nullptr) {
            continue;
        }
        auto bcc = std::make_shared<BCC>();
        for (int s_node: r_node->get_children()) {
            bcc->insert(s_node);
        }
        if (r_node->parent_ != nullptr) {
            bcc->insert(r_node->parent_->id_);
        }
        bccs.push_back(bcc);
    }
}

void BlockCutTree::just_pop_edge() {
    edges_.pop_front();
}

void BlockCutTree::link(BlockCutTreeNode *a, BlockCutTreeNode *b) {
    assert(a != nullptr && b != nullptr);
    assert(a->type_ != b->type_);

    auto square_node = a;
    auto round_node = b;

    assert(square_node != nullptr);
    if (block_cut_tree::SQUARE != square_node->type_) {
        std::swap(square_node, round_node);
    }

    if (square_node->parent_ != nullptr) {
        evert(square_node);
        assert(square_node->parent_ == nullptr);
    }
    square_node->parent_ = round_node;

    round_node->get_children().insert(square_node->id_);
}

BlockCutTreeNode *BlockCutTree::new_round_node() {
    assert(!free_round_nodes_.empty());
    auto rn = free_round_nodes_.back();
    free_round_nodes_.pop_back();
    return rn;
}

void BlockCutTree::evert(BlockCutTreeNode *node) {
    assert(node != nullptr);
    if (node->parent_ == nullptr) {
        return;
    }

    auto p = node->parent_;
    evert(p);

    assert(p->parent_ == nullptr);
    p->parent_ = node;
    node->parent_ = nullptr;

    p->get_children().erase(node->id_);
    node->get_children().insert(p->id_);
}