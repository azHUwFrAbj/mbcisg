

#ifndef EFFICIENCY_DATA_H
#define EFFICIENCY_DATA_H
#include <vector>
#include "../utils.h"


class Timer {
    std::vector<double> running_times;

    TimePoint start_time = std::chrono::high_resolution_clock::now();

public:
    void start();

    void add(double t);

    void collect();

    double collect(double offset_ms);

    const std::vector<double> &times() const {
        return running_times;
    }

    std::vector<double> get_status() const;

    void status() const;

    double get_average() const;
};


#endif