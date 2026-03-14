

#ifndef NEIGHBOURPARTITION_H
#define NEIGHBOURPARTITION_H
#include "my_types.h"


class NeighbourPartition {
#ifdef BCC_DEBUG
public:
#endif

    using PartitionArr = std::vector<VSetP>;
    using PartitionArrP = std::shared_ptr<PartitionArr>;
    using NbPartitions = std::unordered_map<VId, PartitionArrP>;

    using Vertex2NbPartition = std::vector<std::pair<bool, NbPartitions> >;

    Vertex2NbPartition vertex2NbPartition;


    const static VSetP not_a_partition;

    int lmax_ = 0;

public:
    NeighbourPartition(int max_vid) {
        for (int i = 0; i < max_vid + 2; i++) {
            std::pair<bool, NbPartitions> pair = std::make_pair<bool, NbPartitions>(false, {});
            vertex2NbPartition.push_back(pair);
        }
    };

    double get_index_size_in_KB();

    void init(VId v, VId w, int lmax);


    void remove_from_neighbours(VId v, VId u, int lmax);

    VSetP getNbp(VId v, VId w, int i);

    bool isNbp(VId v, VId w, int i) const;

    VSetP get_nbrs_at_level(VId vid, int l);

    bool static is_equal(VSetP xp, VSetP zp);

    bool isTransitiveWedge(VId x, VId y, VId z, int l);

    void set(VId v, VId w, int l, VSetP nbp);

    void unionNbp(VId x, VId y, VId z, int l);

    void unionNbpFuzzy(VId x, VId y, VId z, int l);

private:
    PartitionArrP getPartitionArr(VId v, VId w);

    void init_vw(VId v, VId w, int lmax);

    std::pair<VSetP, VSetP> unite(VId x, VId y, VId z, int l);

    void remove_from_nbr_of(VId v, VId u, int lmax);
};


#endif