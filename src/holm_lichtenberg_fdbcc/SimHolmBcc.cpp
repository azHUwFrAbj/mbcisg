

#include "SimHolmBcc.h"

#include <cassert>
#include <cmath>
#include <vector>
#include <filesystem>

#include "DisjointSet.h"
#include "SimpleTree.h"


double SimHolmBcc::get_index_size_in_KB() {
    return tree.get_index_size_in_KB() + levelInfo.get_index_size_in_KB() + edgeInfo.get_index_size_in_KB() +
           nbrPartition.get_index_size_in_KB();
}

SimHolmBcc::SimHolmBcc(int vertex_count) : vertex_count(vertex_count),
                                           lmax(static_cast<int>(
                                                    std::ceil(std::log(vertex_count) / std::log(2))) + 2),
                                           levelInfo(lmax), notYetMergeSet(lmax),
                                           bccInfo(lmax), nbrPartition(vertex_count) {
    tree.create_toptree(vertex_count);
    std::cout << "Initialization for SimHolmBcc for " << vertex_count << " vertices done." << std::endl;
}

bool SimHolmBcc::biconnected(VId v, VId w) {
    return SimHolmBcc::biconnected(v, w, 0);
}

bool SimHolmBcc::is_nbr(VId v, VId u) {
    auto nbp = get_nbp(v, u, 0);
    return nbp != nullptr && nbp->contains(u);
}

bool SimHolmBcc::biconnected(VId u, VId v, int l) {
    if (u == v || is_nbr(u, v)) {
        return true;
    }

    VArrP path = find_path(u, v);
    if (path->size() > 2) {
        for (int i = 0; i < path->size() - 2; i++) {
            if (!biconnected({path->at(i), path->at(i + 1), path->at(i + 2)}, l)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool SimHolmBcc::biconnected(Wedge w, int l) {
    int x = w.x;
    int y = w.pivot;
    int z = w.z;
    auto X = get_nbp(y, x, l);
    auto Y = get_nbp(y, z, l);
    return NeighbourPartition::is_equal(X, Y);
}

void SimHolmBcc::insert_edge(VId v, VId w) {
#ifdef BCC_DEBUG
    std::cout << "insert_edge: " << v << ' ' << w << std::endl;
#endif
    if (!tree.is_connected(v, w)) {
        tree.link(v, w);
        init_edge(v, w);
    } else {
        init_edge(v, w);
        inc_level_of_nt_edge(v, w, 0);
        cover(v, w, 0);
    }
}

void SimHolmBcc::get_bccs_dfs(std::vector<vector<VId> > &biconnected_components, SimpleTree &skeleton) {
    biconnected_components.clear();

    vector<VId> dfs_stack;
    vector<VId> last_biconnected_nbr;
    for (int vid = 0; vid <= vertex_count; vid++) {
        const auto &nbrs = skeleton.nbrs[vid];
        while (!nbrs.empty()) {
            VId nb = *nbrs.begin();

            dfs_stack.push_back(nb);
            last_biconnected_nbr.push_back(vid);

            vector<VId> bcc;
            bcc.push_back(vid);

            while (!dfs_stack.empty()) {
                VId cur_vid = dfs_stack.back();
                VId last_nb = last_biconnected_nbr.back();

                skeleton.cut(cur_vid, last_nb);
                bcc.push_back(cur_vid);

                dfs_stack.pop_back();
                last_biconnected_nbr.pop_back();

                const auto &cur_nbrs = skeleton.nbrs.at(cur_vid);
                const auto &cnbp = get_nbp(cur_vid, last_nb, 0);
                for (auto nbr: cur_nbrs) {
                    const auto &nbp = get_nbp(cur_vid, nbr, 0);
                    if (NeighbourPartition::is_equal(nbp, cnbp)) {
                        dfs_stack.push_back(nbr);
                        last_biconnected_nbr.push_back(cur_vid);
                    }
                }
            }
            biconnected_components.push_back(bcc);
        }
    }
}

void SimHolmBcc::get_bcc_of_vertex(std::vector<std::vector<VId> > &biconnected_components, VId query_vertex,
                                   SimpleTree &skeleton) {
    biconnected_components.clear();

    vector<VId> dfs_stack;
    vector<VId> last_biconnected_nbr;

    VId vid = query_vertex;
    const auto &nbrs = skeleton.nbrs[vid];
    while (!nbrs.empty()) {
        VId nb = *nbrs.begin();

        dfs_stack.push_back(nb);
        last_biconnected_nbr.push_back(vid);

        vector<VId> bcc;
        bcc.push_back(vid);

        while (!dfs_stack.empty()) {
            VId cur_vid = dfs_stack.back();
            VId last_nb = last_biconnected_nbr.back();

            skeleton.cut(cur_vid, last_nb);
            bcc.push_back(cur_vid);

            dfs_stack.pop_back();
            last_biconnected_nbr.pop_back();

            const auto &cur_nbrs = skeleton.nbrs[cur_vid];
            const auto &cnbp = get_nbp(cur_vid, last_nb, 0);
            for (auto nbr: cur_nbrs) {
                const auto &nbp = get_nbp(cur_vid, nbr, 0);
                if (NeighbourPartition::is_equal(nbp, cnbp)) {
                    dfs_stack.push_back(nbr);
                    last_biconnected_nbr.push_back(cur_vid);
                }
            }
        }
        biconnected_components.push_back(bcc);
    }
}


SimpleTree SimHolmBcc::get_skeleton() {
    SimpleTree skeleton(vertex_count, true);
    vector<Edge> ve;
    tree.get_all_edges(ve);
    for (int i = 0; i < ve.size(); i++) {
        skeleton.link(ve[i].first, ve[i].second);
    }
    return skeleton;
}

void SimHolmBcc::get_bccs(std::vector<VSetP> &biconnected_components) {

    biconnected_components.clear();

    BasicTree basic_tree(vertex_count);
    vector<Edge> ve;
    ve.clear();
    tree.get_all_edges(ve);
    for (int i = 0; i <= vertex_count; i++) {
        basic_tree.add_node(i);
    }
    for (int i = 0; i < ve.size(); i++) {
        basic_tree.link(ve[i].first, ve[i].second);
    }

    DisjointSet disjoint_set(vertex_count + 1);
    for (int vid = 0; vid <= vertex_count; vid++) {
        auto v = basic_tree.get_node(vid);
        if (v->parent != nullptr) {
            VId parent_id = v->parent->id;
            VId grandparent_id = v->parent->parent != nullptr ? v->parent->parent->id : -1;
            auto nbp = get_nbp(parent_id, v->id, 0);
            for (auto z: *nbp) {
                if (z == v->id) {
                    continue;
                }
                if (basic_tree.is_edge(z, parent_id)) {
                    if (z == grandparent_id) {
                        disjoint_set.union_set(v->id, parent_id);
                    } else {
                        disjoint_set.union_set(v->id, z);
                    }
                }
            }
        }
    }

    auto tree_ccs = disjoint_set.get_all_sets();
    for (const auto &tcc: tree_ccs) {
        if (!tcc.empty()) {
            auto bcc = make_VSet();
            for (auto tnode: tcc) {
                auto node = basic_tree.get_node(tnode);
                bcc->insert(node->id);
                if (node->parent != nullptr) {
                    bcc->insert(node->parent->id);
                }
            }
            if (bcc->size() > 1) {
                biconnected_components.push_back(bcc);
            }
        }
    }
}

int SimHolmBcc::get_level(VId v, VId w) {
    return tree.is_edge(v, w) ? edgeInfo.get_cover_level(v, w) : edgeInfo.get_level(v, w);
}

void SimHolmBcc::delete_edge(VId v, VId w) {
    assert(is_nbr(v, w) && "deleted non-existing edge");
#ifdef BCC_DEBUG
    std::cout << "Deleting edge: " << v << ' ' << w << std::endl;
#endif
    deletion_flag = true;
    int l = get_level(v, w) >= 0 ? get_level(v, w) : 0;
    if (tree.is_edge(v, w)) {
        if (!edgeInfo.is_rep_edge(v, w)) {
            release_edge(v, w);
            return;
        }
        this->swap(v, w);
    }

    uncover(v, w, l);
    dec_level_of_nt_edge(v, w, l);
    for (int i = l; i >= 0; i--) {
        recover(v, w, i, false);
    }

    deletion_flag = false;
}

void SimHolmBcc::init_edge(VId v, VId w) {
    nbrPartition.init(v, w, lmax);
    if (tree.is_edge(v, w)) {
        edgeInfo.set_cover_level(v, w, -1);
    }
}

void SimHolmBcc::release_edge(VId v, VId w) {
    tree.cut(v, w);
    nbrPartition.remove_from_neighbours(v, w, lmax);
    nbrPartition.remove_from_neighbours(w, v, lmax);
}

void SimHolmBcc::cover(VId v, VId w, int l) {
    auto path = find_path(v, w);
    cover(v, w, path, l);
}

void SimHolmBcc::cover(VId q, VId r, const VArrP &affected_path, int l) {
    auto path = find_path(q, r);
#ifdef BCC_DEBUG
    std::cout << l << "-covering " << str(affected_path) << " via " << str(path) << std::endl;
#endif
    if (path->size() > 2) {
        auto sv = path->at(1);
        auto sw = path->at(path->size() - 2);
        for (int i = 1; i < path->size() - 1; i++) {
            cover_wedge({path->at(i - 1), path->at(i), path->at(i + 1)}, l, affected_path);
        }
        cover_wedge({r, q, sv}, l, affected_path);
        cover_wedge({q, r, sw}, l, affected_path);
    }
    ecc_cover(path, l);
}

void SimHolmBcc::inc_level_of_nt_edge(VId v, VId w, int l) {
    levelInfo.addEdge(v, w, l);
}

void SimHolmBcc::dec_level_of_nt_edge(VId v, VId w, int l) {
    levelInfo.removeEdge(v, w, l);
}

void SimHolmBcc::swap(VId v, VId w) {
    assert(tree.is_edge(v, w) && "swapping non-existing tree edge");
    assert(edgeInfo.is_rep_edge(w, v) && "swapping a bridge edge");
    int alpha = edgeInfo.get_cover_level(v, w);
    auto rep_edge = edgeInfo.get_rep_edge(v, w);
    VId x = rep_edge.first;
    VId y = rep_edge.second;
    assert(x != -1 && y != -1 && "swapping non-existing edge");

    bccInfo.swap(holm::make_edge(v, w), rep_edge, alpha);

    tree.cut(v, w);
    tree.link(x, y);
    edgeInfo.set_cover_level(x, y, -1);
    ecc_cover(v, w, alpha);
}

void SimHolmBcc::uncover_wedge(Wedge w, int i) {
    if (biconnected(w, i + 1)) {
        return;
    }

    int x = w.x;
    int y = w.pivot;
    int z = w.z;

    auto X = get_nbp(y, x, i + 1);
    auto Z = get_nbp(y, z, i + 1);
    auto XUZ = make_VSet();
    XUZ->insert(X->begin(), X->end());
    XUZ->insert(Z->begin(), Z->end());
    for (int j = i; j >= 0; j--) {
        auto Xj = get_nbp(y, x, j);
        auto Zj = get_nbp(y, z, j);
        auto M_Star = make_VSet();
        M_Star->insert(Xj->begin(), Xj->end());
        auto it = XUZ->begin();
        while (!M_Star->empty() && it != XUZ->end()) {
            M_Star->erase(*it);
            ++it;
        }
        set_nym(w, j, M_Star);

        deep_set_nbp(y, j, X);
        deep_set_nbp(y, j, Z);

        Xj->erase(x);
        Zj->erase(z);


        for (auto s: *M_Star) {
            auto new_S = make_VSet();
            auto S = get_nbp(y, s, i + 1);
            deep_set_nbp(y, j, S);
        }
    }
}

void SimHolmBcc::uncover(VId v, VId w, int l) {
    auto path = find_path(v, w);
#ifdef BCC_DEBUG
    std::cout << "\tuncovering " << str(path) << std::endl;
#endif
    if (path->size() < 2) {
        return;
    }
    for (auto i = 1; i < path->size() - 1; i++) {
        VId u = path->at(i - 1);
        VId v = path->at(i);
        VId w = path->at(i + 1);
        uncover_wedge({u, v, w}, l);
    }

    for (int j = 0; j <= lmax; j++) {
        get_nbp(v, w, j)->erase(w);
        get_nbp(w, v, j)->erase(v);
        set_nbp(v, w, j, nullptr);
        set_nbp(w, v, j, nullptr);
    }

    ecc_uncover(path, l);
}

void SimHolmBcc::ecc_uncover(const VArrP &path, int l) {
    for (auto i = 1; i < path->size(); i++) {
        VId u = path->at(i - 1);
        VId v = path->at(i);
        edgeInfo.set_cover_level(u, v, -1);
        edgeInfo.set_rep_edge(u, v, EdgeInfo::INVALID);
        bccInfo.remove_from_bcc(holm::make_edge(u, v), l);
    }
}

std::vector<std::pair<int, int> > SimHolmBcc::find_non_tree_edges_at_level(VId x, VId u, int l) {
    std::vector<std::pair<int, int> > results;

    if (x == u) {
        auto nbrs = get_nbrs(u, l);
        for (auto v: *nbrs) {
            if (!tree.is_edge(u, v)) {
                results.push_back({u, v});
            }
        }
        return results;
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::set<VId> visited;
    visited.insert(u);
    std::stack<VId> stack;
    stack.push(x);
    while (!stack.empty()) {
        int q = stack.top();
        visited.insert(q);
        stack.pop();

        auto nbrs = get_nbrs(q, l);
        for (auto r: *nbrs) {
            if (tree.is_edge(q, r)) {
                if (!visited.contains(r)) {
                    stack.push(r);
                }
            } else {
                if (edgeInfo.get_level(q, r) == l) {
                    results.push_back({q, r});
                }
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    brute_force_search_time_ms += 0.001f * duration.count();

    return results;
}

VSetP SimHolmBcc::get_nbrs(VId id, int i) {
    return nbrPartition.get_nbrs_at_level(id, i);
}

bool SimHolmBcc::recover_star(VId u, VId u_prime, VId v, VId w, int l, bool phrase_2) {
#ifdef BCC_DEBUG
    std::cout << "recover_star " << u << " " << u_prime << " " << v << " " << w << " " << l << " p" << (
                phrase_2 ? "2" : "1") <<
            std::endl;
#endif

    auto path = find_path(v, w);
    int u_pre = -1;
    int u_post = -1;
    for (auto i = 0; i < path->size(); i++) {
        if (u == path->at(i)) {
            if (i > 0) {
                u_pre = path->at(i - 1);
            }
            if (i < path->size() - 1) {
                u_post = path->at(i + 1);
            }
            break;
        }
    }
    assert(u_pre != -1 || u_post != -1 && "u is not in the path");

    auto started_points = make_VSet();
    started_points->insert(u);
    auto X = get_nbrs(u, l);
    for (auto x: *X) {
        if (tree.is_edge(u, x) && x != u_pre && x != u_post) {
            started_points->insert(x);
        }
    }

    bool illegal_flag = false;
    for (auto x: *started_points) {
        if (u != x && edgeInfo.get_cover_level(x, u) < l) {
            continue;
        }
        auto qrs = find_non_tree_edges_at_level(x, u, l);
        for (auto qr: qrs) {
            int q = qr.first;
            int r = qr.second;
            VSetP qr_path = nullptr;
            VArrP qr_vec = nullptr;

            bool coverable = false;

            if (edgeInfo.get_level(q, r) >= l) {
                auto [varr, vset] = find_path_vset(q, r);
                qr_vec = varr;
                qr_path = vset;
                coverable = qr_path->contains(u);
            }


            if (!coverable) {
                continue;
            }

            if (qr_vec == nullptr) {
                qr_vec = find_path(q, r);
            }

            if (legal_to_inc_level(qr_vec, l + 1)) {
                dec_level_of_nt_edge(q, r, l);
                inc_level_of_nt_edge(q, r, l + 1);
                cover(q, r, path, l + 1);
            } else {
                cover(q, r, path, l);
                illegal_flag = true;
                }
        }

        if (illegal_flag) {
            if (!phrase_2) {
#ifdef BCC_DEBUG
                std::cout << "Entering phrase 2 via recover_star" << std::endl;
#endif
                recover_phrase2(v, w, l);
            }
            return true;
        }
    }

    return false;
}

void SimHolmBcc::recover(const VId v, const VId w, const int l, const bool phrase_2) {
#ifdef BCC_DEBUG
    std::cout << "recover " << v << " " << w << " " << l << std::endl;
#endif
    auto path = find_path(v, w);
    for (auto i = 0; i < path->size(); i++) {
        auto u = path->at(i);
#ifdef BCC_DEBUG
        std::cout << "u ==>" << u << std::endl;
#endif

        if (u != v) {
            #ifdef BCC_DEBUG
            std::cout << "recover (1) u ==>" << u << std::endl;
#endif
            VId sv = path->at(i - 1);

            if (recover_star(u, sv, v, w, l, phrase_2)) {
                return;
            }
        }
        if (u != v && u != w) {
            #ifdef BCC_DEBUG
            std::cout << "recover (2) u ==>" << u << std::endl;
#endif
            VId sv = path->at(i - 1);
            VId sw = path->at(i + 1);

            auto nym = get_nym({sv, u, sw}, l);
            if (!nym->empty()) {

                auto SW = get_nbp(u, sw, l);
                SW->insert(nym->begin(), nym->end());
                for (auto s: *nym) {
                    set_nbp(u, s, l, SW);
                }
#ifdef BCC_DEBUG
                std::cout << "\tSetting " << str(nym) << "'s NBP as" << str(SW) << std::endl;
#endif
                nym->clear();
            }
        }
        if (u != w) {
#ifdef BCC_DEBUG
            std::cout << "recover (3) u ==>" << u << std::endl;
#endif
            VId sw = path->at(i + 1);
            if (recover_star(u, sw, v, w, l, phrase_2)) {
                return;
            }
        }
    }
    if (!phrase_2) {
#ifdef BCC_DEBUG
        std::cout << "Entering phrase 2 not via recover star" << std::endl;
#endif

        recover_phrase2(v, w, l);
    }
}

void SimHolmBcc::recover_phrase2(const VId v, const VId w, const int l) {
#ifdef BCC_DEBUG
    std::cout << "Entering recover_phrase2" << std::endl;
#endif

    recover(w, v, l, true);
}


bool SimHolmBcc::legal_to_inc_level(VArrP qr_path, int l) {
    int bound = static_cast<int>(std::ceil(vertex_count / pow(2, l)));
    if (qr_path->size() <= 1) {
        return true;
    }

    int future_vertices_at_level_l = bccInfo.get_new_bcc_size(qr_path, l);
    return future_vertices_at_level_l <= bound;
}

void SimHolmBcc::cover_wedge(Wedge w, int l, const VArrP &affected_path) {
    for (int j = l; j >= 0; j--) {
        auto [f_exists, f_wedge] = find_wedge(w.pivot, affected_path);
        if (f_exists) {
            auto nym = get_nym(f_wedge, j);
            if (nym->empty()) {
                #ifdef BCC_DEBUG
                std::cout << "\t " << j << "-covering wedge " << str(w) << " @Case A: empty not-yet-merge set" <<
                        std::endl;
#endif
                unite_nbp_based_on_flag(w, j);
            } else {
                auto X = get_nbp(w.pivot, w.x, j);
                auto Z = get_nbp(w.pivot, w.z, j);

                bool butterfly = !X->contains(f_wedge.x) && !X->contains(f_wedge.z) && !Z->
                                 contains(f_wedge.z) && Z->
                                 contains(f_wedge.x) && !Z->contains(f_wedge.z);
                if (butterfly) {
                    #ifdef BCC_DEBUG
                    std::cout << "\t " << j << "-covering wedge " << str(w) << " @Case C" <<
                            std::endl;
#endif
                    unite_nbp_based_on_flag(w, j);
                } else {
                    bool of_the_same_bcc = (X->contains(f_wedge.x) && Z->contains(f_wedge.z)) ||
                                           (X->contains(f_wedge.z) && Z->contains(f_wedge.x));
                    if (of_the_same_bcc) {
#ifdef BCC_DEBUG
                        std::cout << "\t " << j << "-covering wedge " << str(w) << " @Case D fully cover" << std::endl;
#endif
                        fully_cover(w, f_wedge, j);
                    } else {
#ifdef BCC_DEBUG
                        std::cout << "\t " << j << "-covering wedge " << str(w) << " @Case D partially cover" <<
                                std::endl;
#endif
                        partially_cover(w, f_wedge, j);
                    }
                }
            }
        } else {
            #ifdef BCC_DEBUG
            std::cout << "\t " << j << "-covering wedge " << str(w) << " @Case A" << std::endl;
#endif
            unite_nbp_based_on_flag(w, j);
        }
    }
}

void SimHolmBcc::ecc_cover(VId x, VId y, int l) {
    const VArrP path = find_path(x, y);
    Edge e = holm::make_edge(path->front(), path->back());
    for (auto i = 1; i < path->size(); i++) {
        VId u = path->at(i - 1);
        VId v = path->at(i);
        if (edgeInfo.get_cover_level(u, v) <= l) {
            edgeInfo.set_cover_level(u, v, l);
            edgeInfo.set_rep_edge(u, v, e);
        }
    }
}


void SimHolmBcc::ecc_cover(const VArrP &path, int l) {
    Edge e = holm::make_edge(path->front(), path->back());
#ifdef BCC_DEBUG
    std::cout << "ecc_cover: " << l << "-covering " << str(path) << std::endl;
#endif

    if (edgeInfo.get_level(e.first, e.second) <= l) {
        edgeInfo.set_level(e.first, e.second, l);
    }

    for (auto i = 1; i < path->size(); i++) {
        VId u = path->at(i - 1);
        VId v = path->at(i);
        if (edgeInfo.get_cover_level(u, v) <= l) {
            edgeInfo.set_cover_level(u, v, l);
            edgeInfo.set_rep_edge(u, v, e);
        }
    }

    bccInfo.cover(path, l);
}

VArrP SimHolmBcc::find_path(int uid, int vid) {
    return tree.find_path(uid, vid);
}

std::pair<VArrP, VSetP> SimHolmBcc::find_path_vset(int uid, int vid) {
    auto res = tree.find_path_vset(uid, vid);
    return res;
}

VSetP SimHolmBcc::get_nbp(VId v, VId w, int i) {
    assert(nbrPartition.isNbp(v, w, i));
    auto nbp = nbrPartition.getNbp(v, w, i);
    if (!nbp->empty())
        return nbp;
    return make_VSet();
}

void SimHolmBcc::set_nbp(VId v, VId w, int l, VSetP nbp) {
    nbrPartition.set(v, w, l, nbp);
}

void SimHolmBcc::deep_set_nbp(VId v, int l, VSetP nbp) {
    auto X = make_VSet();
    X->insert(nbp->begin(), nbp->end());
    for (auto xx: *X) {
        set_nbp(v, xx, l, X);
    }
}

VSetP SimHolmBcc::get_nym(Wedge w, int l) {
    return notYetMergeSet.getNYNbrs(w.x, w.pivot, w.z, l);
}

void SimHolmBcc::set_nym(Wedge w, int l, VSetP nym) {
#ifdef BCC_DEBUG
    std::cout << "\t " << l << "-set_nym " << str(w) << ", " << nym->size() << std::endl;
#endif
    notYetMergeSet.setNYNbrs(w.x, w.pivot, w.z, l, nym);
}

void SimHolmBcc::unite_nbp(Wedge w, int l) {
#ifdef BCC_DEBUG
    std::cout << "\t SimHolmBcc::unite_nbp: " << l << '-' << str(w) << std::endl;
#endif
    nbrPartition.unionNbp(w.x, w.pivot, w.z, l);
}

void SimHolmBcc::unite_nbp_based_on_flag(Wedge w, int l) {
#ifdef BCC_DEBUG
    std::cout << "\t SimHolmBcc::unite_nbp: " << l << '-' << str(w) << std::endl;
#endif
    if (deletion_flag) {
        nbrPartition.unionNbp(w.x, w.pivot, w.z, l);
    } else {
        nbrPartition.unionNbp(w.x, w.pivot, w.z, l);
    }
}

void SimHolmBcc::fully_cover(Wedge w, Wedge f, int l) {
    unite_nbp(w, l);
    auto z = get_nbp(w.pivot, w.z, l);
    auto nym = get_nym(f, l);
    z->insert(nym->begin(), nym->end());
    for (auto id: *nym) {
        set_nbp(w.pivot, id, l, z);
    }
    nym->clear();
}

void SimHolmBcc::partially_cover(Wedge w, Wedge f, int l) {
    unite_nbp(w, l);
    auto z = get_nbp(w.pivot, w.z, l);
    auto nym = get_nym(f, l);
    set_minors(nym, z);
}

std::pair<bool, Wedge> SimHolmBcc::find_wedge(VId p, const VArrP &path) {
    if (path->size() < 2) {
        return {false, Wedge(-1, -1, -1)};
    }
    for (int i = 1; i < path->size() - 1; i++) {
        if (path->at(i) == p) {
            return {true, Wedge(path->at(i - 1), path->at(i), path->at(i + 1))};
        }
    }
    return {false, Wedge(-1, -1, -1)};
}