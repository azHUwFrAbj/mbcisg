

#include "../BcTreeQ.h"

#include <cassert>

BcTreeQ::BcTreeQ(int N) : bc_tree_(N) {
}

void BcTreeQ::insert(Edge e, int timestamp) {
    timestamps.set_value(e.first, e.second, timestamp);
    bc_tree_.insert_update(e.first, e.second);
}

void BcTreeQ::expire_edges(const std::vector<Edge> &edges, int expired_time) {
    delete_edges(edges);
}

bool BcTreeQ::bcc_query(VId vid, VId wid) {
    return bc_tree_.bcc_query(vid, wid);
}

void BcTreeQ::delete_edges(const std::vector<Edge> &edges) {
    std::set<VId> affected_ccs;
    for (auto edge: edges) {
        if (!affected_ccs.contains(edge.first) && !affected_ccs.contains(edge.second)) {
            auto cc = bc_tree_.get_cc(edge.first);
            affected_ccs.insert(cc.begin(), cc.end());
        }

        auto front_edge = bc_tree_.front_edge();
        assert(front_edge == edge && "Out of ordered deletion!");
        bc_tree_.pop_front_edge();
    }

    if (!affected_ccs.empty()) {
        bc_tree_.build(affected_ccs, edges);
    }
}

void BcTreeQ::get_bccs(BCCs &bccs) {
    bc_tree_.get_bccs(bccs);
}

void BcTreeQ::get_bcc(Edge e, BCC &bcc) {
    bc_tree_.get_bcc(e, bcc);
}

void BcTreeQ::add_edge_without_maintaining_bc_tree(int vid, int wid) {
    bc_tree_.just_add_edge(vid, wid);
}

void BcTreeQ::expire_edge_without_maintaining_bc_tree() {
    bc_tree_.just_pop_edge();
}

void BcTreeQ::build_bc_tree() {
    bc_tree_.build();
}

void BcTreeQ::run_ht_algorithm(std::vector<std::vector<VId> > &bbcs) {
    bc_tree_.run_ht_algorithm(bbcs);
}

void BcTreeQ::run_ht_algorithm_label_based() {
    bc_tree_.run_ht_algorithm_label_based();
}