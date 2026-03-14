

#ifndef SIMHOLMBCC_H
#define SIMHOLMBCC_H
#include <iostream>
#include <ostream>
#include "top_tree.h"
#include "tree.h"
#include "BasicTree.h"
#include "BCCInfo.h"
#include "EdgeInfo.h"
#include "LevelInfo.h"
#include "NeighbourPartition.h"
#include "NotYetMergeSet.h"
#include "SimpleTree.h"


class SimHolmBcc {
public:
    const int vertex_count;
    const int lmax;

    double brute_force_search_time_ms = 0.0;

private:
#ifdef BCC_DEBUG

public:
#endif

    TopTree tree;
    EdgeInfo edgeInfo;
    LevelInfo levelInfo;
    NeighbourPartition nbrPartition;
    NotYetMergeSet notYetMergeSet;
    BCCInfo bccInfo;

    bool deletion_flag = false;

public:
    double get_index_size_in_KB();

    SimHolmBcc() = delete;

    explicit SimHolmBcc(int vertex_count);

    bool biconnected(VId v, VId w);

    void get_bccs(std::vector<VSetP> &biconnected_components);

    void insert_edge(VId v, VId w);

    void get_bccs_dfs(std::vector<vector<VId> > &biconnected_components, SimpleTree &spanning_tree);

    void get_bcc_of_vertex(std::vector<std::vector<VId> > &bbcs, VId query_vertex, SimpleTree &spanning_tree);


    SimpleTree get_skeleton();

    void delete_edge(VId v, VId w);


#ifndef BCC_DEBUG

private:
#endif

    int get_level(VId v, VId w);

    bool is_nbr(VId v, VId u);

    bool biconnected(VId u, VId v, int l);

    bool biconnected(Wedge w, int l);

    void init_edge(VId v, VId w);

    void release_edge(VId v, VId w);

    VArrP find_path(int uid, int vid);

    std::pair<VArrP, VSetP> find_path_vset(int uid, int vid);

    VSetP get_nbp(VId v, VId w, int i);

    void set_nbp(VId v, VId w, int l, VSetP nbp);

    void deep_set_nbp(VId v, int l, VSetP nbp);

    VSetP get_nym(Wedge w, int l);

    void set_nym(Wedge w, int l, VSetP nym);

    void unite_nbp(Wedge w, int l);

    void unite_nbp_based_on_flag(Wedge w, int l);

    void fully_cover(Wedge w, Wedge f, int l);

    void partially_cover(Wedge w, Wedge f, int l);

    static std::pair<bool, Wedge> find_wedge(VId p, const VArrP &path);

    void cover_wedge(Wedge w, int l, const VArrP &affected_path);

    void ecc_cover(VId x, VId y, int l);

    void ecc_cover(const VArrP &path, int l);

    void cover(VId v, VId w, int l);

    void cover(VId q, VId r, const VArrP &affected_path, int l);

    void inc_level_of_nt_edge(VId v, VId w, int l);

    void dec_level_of_nt_edge(VId v, VId w, int l);

    void swap(VId v, VId w);

    void uncover_wedge(Wedge w, int i);

    void uncover(VId v, VId w, int i);

    void ecc_uncover(const VArrP &path, int l);

    std::vector<std::pair<int, int> > find_non_tree_edges_at_level(VId x, VId u, int l);

    VSetP get_nbrs(VId id, int i);

    bool recover_star(VId u, VId u_prime, VId v, VId w, int l, bool phrase_2);

    void recover(VId v, VId w, int l, bool phrase_2);

    void recover_phrase2(VId v, VId w, int l);

    bool legal_to_inc_level(VArrP qr_path, int l);
};


#endif