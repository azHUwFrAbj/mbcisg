

#include "NeighbourPartition.h"

#include <cassert>
#include <iostream>
#include <ostream>
#include <utility>

#include "commons.h"

const VSetP NeighbourPartition::not_a_partition = make_VSet();


void NeighbourPartition::init_vw(VId v, int w, int lmax) {
    if (vertex2NbPartition[v].first == false) {
        vertex2NbPartition[v].first = true;
    }
    auto &nbp = vertex2NbPartition[v].second;

    auto vec = std::make_shared<PartitionArr>();
    for (int l = 0; l <= lmax; l++) {
        auto s = make_VSet();
        s->insert(w);
        vec->push_back(s);
    }

    if (!nbp.contains(w)) {
        nbp.insert({w, vec});
    } else {
        nbp.at(w) = vec;
    }
}

std::pair<VSetP, VSetP> NeighbourPartition::unite(VId x, VId y, VId z, int l) {
    auto X = getNbp(y, x, l);
    auto Z = getNbp(y, z, l);

    if (is_equal(X, Z)) {
#ifdef BCC_DEBUG
        std::cout << "\t Uniting the same NBP" << str(X) << std::endl;
#endif
        return {nullptr, nullptr};
    }

#ifdef BCC_DEBUG
    std::cout << "\t Uniting " << str(X) << " and "
            << str(Z) << std::endl;
#endif
    if (X->size() > Z->size()) {
        std::swap(X, Z);
    }
    Z->insert(X->begin(), X->end());

    return {X, Z};
}

double NeighbourPartition::get_index_size_in_KB() {
    size_t size = 0;
    std::set<VSetP> visited;
    for (auto &kv: vertex2NbPartition) {
        if (!kv.first) {
            continue;
        }

        size += 1;
        const NbPartitions &partition = kv.second;
        for (auto &nb: partition) {
            const PartitionArrP partArr = nb.second;
            for (const VSetP &s: *partArr) {
                if (!visited.contains(s)) {
                    size += s->size();
                    visited.insert(s);
                }
            }
        }
    }

    return size * 8 / 1024.0;
}

void NeighbourPartition::init(VId v, VId w, int lmax) {
    lmax_ = std::max(lmax_, lmax);
    init_vw(v, w, lmax);
    init_vw(w, v, lmax);
}

void NeighbourPartition::remove_from_nbr_of(VId v, VId u, int lmax) {
    if (vertex2NbPartition.at(v).first) {
        auto parr = getPartitionArr(v, u);
        for (int i = 0; i <= lmax; i++) {
            auto partition = parr->at(i);
            partition->erase(u);
            parr->at(i) = not_a_partition;
        }
    }
}

void NeighbourPartition::remove_from_neighbours(VId v, VId u, int lmax) {
    remove_from_nbr_of(v, u, lmax);
    remove_from_nbr_of(u, v, lmax);
}

VSetP NeighbourPartition::getNbp(VId v, VId w, int i) {
    auto vec = getPartitionArr(v, w);
    return vec->at(i);
}

bool NeighbourPartition::isNbp(VId v, VId w, int i) const {
    if (vertex2NbPartition[v].first) {
        const auto &nbp = vertex2NbPartition.at(v).second;
        if (nbp.find(w) != nbp.end()) {
            return true;
        }
    }
    return false;
}

VSetP NeighbourPartition::get_nbrs_at_level(VId vid, int l) {
    if (vertex2NbPartition[vid].first) {
        const auto &nbp = vertex2NbPartition.at(vid).second;
        VSetP keys = make_VSet();
        for (const auto &p: nbp) {
            if (p.second->size() >= l && !p.second->at(l)->empty()) {
                keys->insert(p.first);
            }
        }
        return keys;
    }
    throw std::runtime_error("Not indexed partition");
}

bool NeighbourPartition::is_equal(VSetP xp, VSetP zp) {
    if (xp != not_a_partition && zp != not_a_partition) {
        return xp == zp;
    }
    return false;
}

bool NeighbourPartition::isTransitiveWedge(VId x, VId y, VId z, int l) {
    auto xp = getNbp(y, x, l);
    auto zp = getNbp(y, z, l);

    return is_equal(xp, zp);
}

void NeighbourPartition::set(VId v, VId w, int l, VSetP nbp) {
    auto nbpArr = getPartitionArr(v, w);
    if (nbp == nullptr) {
        nbp = not_a_partition;
    }
#ifdef BCC_DEBUG
    std::cout << "\t\tNeighbourPartition::set " << v << " " << w << " " << l << " " << str(nbp) << std::endl;
#endif
    nbpArr->at(l) = nbp;
}

void NeighbourPartition::unionNbp(VId x, VId y, VId z, int l) {
    auto [X,Z] = unite(x, y, z, l);
    if (X == nullptr || Z == nullptr) {
        return;
    }

    for (auto xx: *X) {
        set(y, xx, l, Z);
    }
}

void NeighbourPartition::unionNbpFuzzy(VId x, VId y, VId z, int l) {
    auto [X,Z] = unite(x, y, z, l);
    if (X == nullptr || Z == nullptr) {
        return;
    }

    set(y, x, l, Z);
}

NeighbourPartition::PartitionArrP NeighbourPartition::getPartitionArr(VId v, VId w) {
    if (vertex2NbPartition[v].first) {
        auto &nbp = vertex2NbPartition.at(v).second;
        if (nbp.find(w) == nbp.end()) {
            init(v, w, lmax_);
        }
        return nbp.at(w);
    }
    throw std::runtime_error("Not indexed partition");
}