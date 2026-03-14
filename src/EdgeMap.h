

#ifndef EDGEMAP_H
#define EDGEMAP_H

#include "utils.h"


struct EdgeHashFunc {
    std::size_t operator()(const Edge &edge) const {
        auto h1 = std::hash<VId>{}(edge.first);
        auto h2 = std::hash<VId>{}(edge.second);
        return h1 ^ (h2 << 1);
    }
};

template<class ValueT>
class EdgeMap {
private:
    std::unordered_map<Edge, ValueT, EdgeHashFunc> m;

public:
    void set_value(VId vid, VId wid, const ValueT &value);

    ValueT at(const Edge &edge) const;

    ValueT at(VId vid, VId wid) const;

    void erase(VId vid, VId wid);

    bool contains(VId vid, VId wid) const;

    std::vector<ValueT> get_values() const;
};


template<class ValueT>
void EdgeMap<ValueT>::set_value(VId vid, VId wid, const ValueT &value) {
    auto key = make_edge(vid, wid);
    if (this->contains(key.first, key.second)) {
        m[key] = value;
    } else {
        m.insert({key, value});
    }
}

template<class ValueT>
ValueT EdgeMap<ValueT>::at(const Edge &edge) const {
    auto key = make_edge(edge.first, edge.second);
    return m.at(key);
}

template<class ValueT>
ValueT EdgeMap<ValueT>::at(VId vid, VId wid) const {
    auto key = make_edge(vid, wid);
    return m.at(key);
}

template<class ValueT>
void EdgeMap<ValueT>::erase(VId vid, VId wid) {
    auto key = make_edge(vid, wid);
    m.erase(key);
}

template<class ValueT>
bool EdgeMap<ValueT>::contains(VId vid, VId wid) const {
    auto key = make_edge(vid, wid);
    return m.find(key) != m.end();
}

template<class ValueT>
std::vector<ValueT> EdgeMap<ValueT>::get_values() const {
    std::vector<ValueT> values;
    for (auto it = m.begin(); it != m.end(); ++it) {
        values.push_back(it->second);
    }
    return values;
}
#endif