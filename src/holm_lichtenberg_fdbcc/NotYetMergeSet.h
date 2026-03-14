

#ifndef NOTYETMERGESET_H
#define NOTYETMERGESET_H

#include <iostream>

#include "commons.h"
#include "my_types.h"


class NotYetMergeSet {
#ifdef BCC_DEBUG

public:
#endif
    using NotYetMergePair = std::pair<VId, VId>;

    struct NotYetMergePairHash {
        std::size_t operator()(const NotYetMergePair &xz) const {
            auto h1 = std::hash<VId>{}(xz.first);
            auto h2 = std::hash<VId>{}(xz.second);
            return h1 ^ (h2 << 1);
        }
    };

    static NotYetMergePair makeNbPair(VId v1, VId v2) {
        return make_ordered_pair(v1, v2);
    }

    using NYNbrs = std::unordered_map<NotYetMergePair, VSetP, NotYetMergePairHash>;
    using NYNbrsMap = std::unordered_map<VId, NYNbrs>;
    std::vector<NYNbrsMap> nymergedArr;
    int maxLayer;

public:
    NotYetMergeSet() = delete;

    NotYetMergeSet(int lmax) : maxLayer(lmax + 1), nymergedArr(lmax + 1) {
    };

    bool empty() const {
        for (const auto &n: nymergedArr) {
            for (const auto &p: n) {
                for (const auto &q: p.second) {
                    if (!q.second->empty()) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    NYNbrs &getNYNbrs(VId y, int l) {
        auto &nyNbr = nymergedArr[l];
        if (!nyNbr.contains(y)) {
            nyNbr[y] = {};
        }
        return nyNbr.at(y);
    }

    VSetP getNYNbrs(VId x, VId y, VId z, int l) {
#ifdef BCC_DEBUG
        std::cout << "\t ==>getting Not-Yet Merge Set for " << str(Wedge{x, y, z}) << " at level " << l;
#endif
        NYNbrs &nyNbrs = getNYNbrs(y, l);
        NotYetMergePair npy = makeNbPair(x, z);
        if (!nyNbrs.contains(npy)) {
            nyNbrs[npy] = make_VSet();
        }
        auto result = nyNbrs.at(npy);
#ifdef BCC_DEBUG
        std::cout << "-> " << str(result) << std::endl;
#endif
        return result;
    }

    void setNYNbrs(VId x, VId y, VId z, int l, VSetP vs) {
#ifdef BCC_DEBUG
        std::cout << "\t==> setting nym: " << x << " " << y << " " << z << " " << l << std::endl;
#endif
        NYNbrs &nyNbrs = getNYNbrs(y, l);
        NotYetMergePair npy = makeNbPair(x, z);
        nyNbrs[npy] = vs;
    }

    void printNotYetMergeSet() const {
        std::cout << "NotYetMergeSet Contents:" << std::endl;
        for (int l = 0; l <= maxLayer; ++l) {
            std::cout << "Layer " << l << ":" << std::endl;

            const auto &layerMap = nymergedArr[l];
            if (!layerMap.empty()) {
                for (const auto &[y, nyNbrs]: layerMap) {
                    std::cout << "  Pivot Vertex " << y << ":" << std::endl;
                    for (const auto &[pair, vset]: nyNbrs) {
                        std::cout << "    Neighbour Pair (" << pair.first << ", " << pair.second << ") ->" << str(vset)
                                <<
                                std::endl;
                    }
                }
            }
        }
    }
};


#endif