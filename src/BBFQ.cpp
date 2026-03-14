

#include "BBFQ.h"

#include <cassert>
#include <iostream>

double BBFQ::get_index_size_in_KB() {
    return mxt_.get_index_size_in_KB() + bbf_.get_index_size_in_KB();
}

int BBFQ::get_bbf_node_count() const {
    return bbf_.count_nodes();
}

int BBFQ::get_bbf_height() const {
    return bbf_.get_bbf_height();
}

void BBFQ::get_bccs_cache_trick(vector<vector<VId> > &vbccs) {
    bbf_.get_bccs_cache_trick(vbccs);
}

void BBFQ::get_tree_edges_timestamps_of(VId vid, vector<int> &tree_edge_timestamps) {
    tree_edge_timestamps.clear();

    for (int i = 0; i < mxt_.N_; i++) {
        assert(mxt_.vid2_nodes_.contains(i));
        auto node = mxt_.vid2_nodes_[i];
        if (node->parent_ == nullptr) {
            continue;
        }
        if (node->vid_ == vid || node->parent_->vid_ == vid) {
            tree_edge_timestamps.push_back(node->timestamp_);
        }
    }
}

void BBFQ::get_bcc_of_tree_edges(std::vector<std::vector<VId> > &bbcs, const vector<int> &tree_edge_timestamps) {
    bbf_.get_bcc_of_tree_edges(bbcs, tree_edge_timestamps);
}

int BBFQ::get_root_count() const {
    return bbf_.get_root_count();
}

void BBFQ::fresh_roots_vec() {
    bbf_.fresh_roots_vec();
}

bool BBFQ::insert_basic(const Edge &new_edge, int timestamp) {
    std::vector<TEdge> path;
    TEdge victim = mxt_.insert_edge_and_get_t_path(new_edge, timestamp, path);

    bbf_.insert_edge_basic(new_edge, timestamp, victim, path);

    return !path.empty();
}

void BBFQ::insert_advance(const Edge &new_edge, int timestamp) {
    std::vector<TEdge> path;
    TEdge victim = mxt_.insert_edge_and_get_t_path(new_edge, timestamp, path);
    bbf_.insert_edge_adv(new_edge, timestamp, victim, path);
}

void BBFQ::insert_adv_with_cache(const Edge &new_edge, int timestamp) {
    std::vector<TEdge> path;
    TEdge victim = mxt_.insert_edge_and_get_t_path(new_edge, timestamp, path);
    bbf_.insert_edge_adv_wc(new_edge, timestamp, victim, path);
}

void BBFQ::insert_advance_qo(const Edge &new_edge, int timestamp) {
    std::vector<TEdge> path;
    TEdge victim = mxt_.insert_edge_and_get_t_path(new_edge, timestamp, path);
    bbf_.insert_edge_adv_oq(new_edge, timestamp, victim, path);

    }

void BBFQ::insert_batch(const std::vector<Edge> &new_edges, int first_timestamp) {
#ifndef NDEBUG

    int timestamp = first_timestamp;
    std::vector<TEdge> path;
    for (const Edge &edge: new_edges) {
        TEdge victim = mxt_.insert_edge_and_get_t_path(edge, timestamp, path);
        bbf_.insert_edge_adv(edge, timestamp, victim, path);
        timestamp++;
    }
#else

    static int batch_size = 1000;
    insert_batch_bottom_up_stepwise(new_edges, first_timestamp, batch_size);
#endif

    assert(bbf_.is_tree_and_no_deleted_root());
    assert(bbf_.is_no_single_child_and_no_deleted_child());
}

void BBFQ::insert_batch_qo(const std::vector<Edge> &new_edges, int first_timestamp) {
    int timestamp = first_timestamp;
    std::vector<TEdge> path;
    for (const Edge &edge: new_edges) {
        TEdge victim = mxt_.insert_edge_and_get_t_path(edge, timestamp, path);
        bbf_.insert_edge_adv_oq(edge, timestamp, victim, path);
        timestamp++;

        }


    assert(bbf_.is_tree_and_no_deleted_root());
    assert(bbf_.is_no_single_child_and_no_deleted_child());
}

void BBFQ::insert_batch_bottom_up_stepwise(const std::vector<Edge> &new_edges, const int first_timestamp,
                                           int batch_size) {
    const size_t N = new_edges.size();
    for (size_t i = 0; i < N; i += static_cast<size_t>(batch_size)) {
        const size_t end = std::min(N, i + static_cast<size_t>(batch_size));

        std::vector<Edge> this_batch;
        this_batch.reserve(end - i);
        this_batch.insert(this_batch.end(), new_edges.begin() + i, new_edges.begin() + end);

        insert_batch_bottom_up(this_batch, first_timestamp + static_cast<int>(i));
    }
    bbf_.clean_deleted_roots();
}

void BBFQ::insert_batch_bottom_up(const std::vector<Edge> &new_edges, const int first_timestamp) {

    BBF::Time_Max_Queue Q;
    int timestamp = first_timestamp;

    int new_max_time = timestamp + (int) new_edges.size() - 1;
    bbf_.t_max_ = bbf_.t_max_ > new_max_time ? bbf_.t_max_ : new_max_time;

    for (auto eg: new_edges) {
        std::vector<TEdge> path;
        TEdge victim = mxt_.insert_edge_and_get_t_path(eg, timestamp, path);

        bbf_.add_isolated_node(eg, timestamp);

        if (!path.empty()) {
            bbf_.build_BEG(victim, path, Q);
        }

        timestamp++;
    }

    bbf_.bottom_up_maintenance(Q);

#ifndef NDEBUG
    bbf_.clean_deleted_roots();
#endif

    assert(bbf_.is_tree_and_no_deleted_root());
    assert(bbf_.is_no_single_child_and_no_deleted_child());
}

void BBFQ::expire(int expired_timestamp) {
    std::vector<Edge> expired_tree_edges;
    bbf_.expire(expired_timestamp, expired_tree_edges);
    for (auto &edge: expired_tree_edges) {
        mxt_.direct_delete(edge.first, edge.second);
    }
}

void BBFQ::expire_bbf(int expired_timestamp, std::vector<Edge> &expired_edges) {
    bbf_.expire(expired_timestamp, expired_edges);
    }

void BBFQ::expire_mxt(std::vector<Edge> &expired_edges) {
    for (auto &edge: expired_edges) {
        mxt_.direct_delete(edge.first, edge.second);
    }
}

void BBFQ::expire_qo(int expired_timestamp) {
    std::vector<Edge> expired_tree_edges;
    bbf_.expire_qo(expired_timestamp, expired_tree_edges);
    for (auto &edge: expired_tree_edges) {
        mxt_.direct_delete(edge.first, edge.second);
    }
}

void BBFQ::get_bccs(BCCs &bccs) {
    bbf_.get_bccs(bccs);
}

void BBFQ::get_bccs(vector<vector<VId> > &bccs) {
    bbf_.get_bccs(bccs);
}

void BBFQ::get_bccs_label_based() {
    bbf_.get_bccs_label();
}


bool BBFQ::is_biconnected(VId vid, VId wid) {
    auto [t1, t2] = mxt_.find_path_edge_timestamp(vid, wid);

    if (t1 < 0 || t2 < 0) {
        return false;
    }
    return bbf_.has_lca(t1, t2);
}

bool BBFQ::is_biconnected_qo(VId vid, VId wid) {
    auto [t1, t2] = mxt_.find_path_edge_timestamp(vid, wid);


    if (t1 < 0 || t2 < 0) {
        return false;
    }

    return bbf_.has_lca_qo(t1, t2);
}

