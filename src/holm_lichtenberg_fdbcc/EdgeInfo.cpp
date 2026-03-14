

#include "EdgeInfo.h"

const Edge EdgeInfo::INVALID = {-1, -1};

double EdgeInfo::get_index_size_in_KB() {
    size_t storage_count = 0;

    storage_count += levelM.size();
    storage_count += coverM.size();

    return storage_count * 8 / 1024.0;
}