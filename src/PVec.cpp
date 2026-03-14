

#include "PVec.h"

std::ostream &operator<<(std::ostream &os, const std::pair<int, int> &p) {
    return os << "(" << p.first << ", " << p.second << ")";
}