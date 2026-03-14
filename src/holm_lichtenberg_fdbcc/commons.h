

#ifndef COMMONS_H
#define COMMONS_H

#ifndef BCC_DEBUG
#endif


#include "my_types.h"

class NotYetMergeSet;

VArrP make_VArr();

VSetP make_VSet();

template<typename T>
std::pair<T, T> make_ordered_pair(T t1, T t2);

template<typename T>
std::pair<T, T> make_ordered_pair(T t1, T t2) {
    return t1 < t2 ? std::make_pair(t1, t2) : std::make_pair(t2, t1);
}


void set_minors(VSetP setA, VSetP setB);


std::vector<Edge> read_edges_from_file(const std::string &filename);

namespace holm {
    int count_vertices(const std::set<Edge> &pairs);

    int count_vertices(const std::vector<Edge> &pairs);

    Edge make_edge(VId v1, VId v2);

    bool contains_bcc(const std::vector<VSetP> &bccs, const std::set<VId> &expected_bcc);
}

void print_bccs(const std::vector<VSetP> &bccs);


std::string str(const VArrP &path);

std::string str(const VSetP &nodes);

std::string str(Wedge w);

std::string str(Edge e);


#endif