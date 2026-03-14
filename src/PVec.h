

#ifndef VECTORP_H
#define VECTORP_H
#include <memory>
#include <sstream>
#include <vector>

std::ostream &operator<<(std::ostream &os, const std::pair<int, int> &p);

template<class T>
class PVec {
private:
    std::shared_ptr<std::vector<T> > vec;

public:
    PVec() {
        vec = std::make_shared<std::vector<T> >();
    }

    void push_back(const T &t) {
        vec->push_back(t);
    }

    size_t size() const {
        return vec->size();
    }

    T &operator[](size_t index) {
        return vec->operator[](index);
    }

    const T &at(size_t index) const {
        return vec->at(index);
    }

    void clear() {
        vec->clear();
    }

    std::string to_string() const {
        std::stringstream ss;
        for (size_t i = 0; i < vec->size(); ++i) {
            ss << vec->operator[](i) << " ";
        }
        return ss.str();
    }

    bool empty() const {
        return vec->empty();
    }

    T &front() {
        return vec->front();
    }

    T &back() {
        return vec->back();
    }
};


#endif