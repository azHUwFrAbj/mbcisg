

#include "Timer.h"

#include <iomanip>
#include <iostream>
#include <numeric>

void Timer::start() {
    start_time = std::chrono::high_resolution_clock::now();
}

void Timer::add(double t) {
    running_times.push_back(t);
}

void Timer::collect() {
    auto duration = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - start_time);
    running_times.push_back(duration.count());
}

double Timer::collect(double offset_ms) {
    auto duration = std::chrono::duration<double, std::micro>(std::chrono::high_resolution_clock::now() - start_time);
    double elapsed_ms = duration.count();
    elapsed_ms += offset_ms;
    running_times.push_back(elapsed_ms);
    return elapsed_ms;
}

std::vector<double> Timer::get_status() const {
    if (running_times.empty()) {
        std::cout << "No recorded times.\n";
        return {0, 0, 0, 0};
    }

    std::vector<double> times = running_times;
    std::sort(times.begin(), times.end());

    auto percentile = [&](double p) {
        size_t idx = static_cast<size_t>(p * times.size());
        idx = std::min(idx, times.size() - 1);
        return times[idx];
    };

    double average = std::accumulate(times.begin(), times.end(), 0.0) / times.size();

    std::cout << std::fixed << std::setprecision(4);
    double avg = average * 0.001;
    double p90 = percentile(0.90);
    double p95 = percentile(0.95);
    double p99 = percentile(0.99);

    return {avg, p90, p95, p99};
}

void Timer::status() const {
    if (running_times.empty()) {
        std::cout << "No recorded times.\n";
        return;
    }

    std::vector<double> times = running_times;
    std::sort(times.begin(), times.end());

    auto percentile = [&](double p) {
        size_t idx = static_cast<size_t>(p * times.size());
        idx = std::min(idx, times.size() - 1);
        return times[idx];
    };

    double average = std::accumulate(times.begin(), times.end(), 0.0) / times.size();

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Average time (ms): " << average * 0.001 << "\n";
    std::cout << "P90 latency (ms): " << percentile(0.90) * 0.001 << "\n";
    std::cout << "P95 latency (ms): " << percentile(0.95) * 0.001 << "\n";
    std::cout << "P99 latency (ms): " << percentile(0.99) * 0.001 << "\n";
}

double Timer::get_average() const {
    std::vector<double> times = running_times;
    double average = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    return average;
}