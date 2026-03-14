

#include "../utils.h"

Edge make_edge(VId v1, VId v2) {
    if (v1 > v2) {
        std::swap(v1, v2);
    }
    return Edge(v1, v2);
}