

#ifndef BCTREEQ_H
#define BCTREEQ_H
#include "BlockCutTree.h"
#include "EdgeMap.h"


class BcTreeQ {
private:
    BlockCutTree bc_tree_;
    EdgeMap<int> timestamps;

public:
    BcTreeQ() = delete;

    explicit BcTreeQ(int N);

    void insert(Edge e, int timestamp);

    void expire_edges(const std::vector<Edge> &edges, int expired_time);

    bool bcc_query(VId vid, VId wid);

    void delete_edges(const std::vector<Edge> &edges);

    void get_bccs(BCCs &bccs);

    void get_bcc(Edge e, BCC &bcc);

    void add_edge_without_maintaining_bc_tree(int vid, int wid);

    void expire_edge_without_maintaining_bc_tree();

    void build_bc_tree();

    void run_ht_algorithm(std::vector<std::vector<VId> > &bbcs);

    void run_ht_algorithm_label_based();

    const std::deque<Edge> &get_edges();
};


#endif