

#include "LevelInfo.h"

double LevelInfo::get_index_size_in_KB() {
    int storage_count = 0;

    for (const auto &p: neighbours_at_level) {
        storage_count += 1;
        storage_count += p.second.size();

        for (const auto &level_nbr_partition: p.second) {
            storage_count += level_nbr_partition.second->size();
        }
    }

    return storage_count * 8 / 1024.0;
}