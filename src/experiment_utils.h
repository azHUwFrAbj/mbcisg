

#ifndef EXPERIMENT_UTILS_H
#define EXPERIMENT_UTILS_H

#include "Timer.h"
#include "../utils.h"
#include <vector>
#include <iomanip>

using namespace std;


struct ExperimentSettings {
    string data_set_path;
    int window_size;
    int stride_size;
    int stride_count;

    std::chrono::system_clock::time_point exp_start_time;
    std::chrono::system_clock::time_point exp_end_time;
};

struct ExperimentRecords {
    ExperimentSettings settings;

    int hit_count = 0;
    int total_count = 0;

    Timer recomputation;
    Timer block_forest;
    Timer holm_fdbcc;
    Timer bbf_basic;
    Timer bbf_advance;
    Timer bbf_batch;
    Timer bbf_advance_with_cache;


    Timer fd_insertion;
    Timer fd_deletion;
    Timer bbf_basic_insertion;
    Timer bbf_basic_deletion;
    Timer bbf_advance_insertion;
    Timer bbf_advance_deletion;
    Timer bbf_advance_insertion_with_cache;
    Timer bbf_advance_deletion_with_cache;


    Timer fd_size;
    Timer bbf_size;
    Timer bbf_height;
    Timer bbf_nodes;

    Timer fd_init_size;
    Timer bbf_init_size;
};

void init(ExperimentRecords &experiment_records, const string &data_set_path, int window_size, int stride_size);

void report(const ExperimentRecords &experiment_records);

void step(ExperimentRecords &experiment_records);

void end(ExperimentRecords &experiment_records);

void damp_record(ExperimentRecords &experiment_records, const string &data_damp);


void read_tsv_file(const string &tsv_file, vector<Edge> &edges);

void read_tsv_file(const string &tsv_file, queue<Edge> &edges);

void evaluate_sw_efficiency_pct(const string &data_dir, const string &tsv_file, double window_percentage,
                                int stride_counts, bool run_fdbcc, const string &damp_dir);

void evaluate_sw_efficiency(const string &data_dir, const string &tsv_file, int window_size, int stride_size,
                            int stride_counts, bool run_fdbcc, const string &damp_dir);

void evaluate_window_size(const string &data_dir, const string &tsv_file, int stride_size,
                          const std::vector<int> &window_sizes, int stride_counts, bool run_fdbcc,
                          const string &damp_dir);

void evaluate_window_size_ptc(const string &data_dir, const string &tsv_file, const std::vector<double> &percentage,
                              int stride_counts,
                              bool run_fdbcc, const string &damp_dir);

void evaluate_query_efficiency(const string &data_dir, const string &tsv_file, int window_size, int stride_size,
                               int query_times, int stride_counts, bool run_fdbcc, const
                               string &damp_dir, double hit_rate);

void evaluate_query_efficiency_pct(const string &data_dir, const string &tsv_file, const vector<double> &percentages,
                                   int query_times, int stride_counts, bool run_fdbcc, const string &damp_dir,
                                   double hit_rate);

void evaluate_search_efficiency_pct(const string &data_dir, const string &tsv_file, double pct, int search_count,
                                    bool run_fdbcc, const string &damp_dir);

void evaluate_search_efficiency(const string &data_dir, const string &tsv_file, int window_size,
                                int search_count, bool run_fdbcc, const string &damp_dir);

void evaluate_retrieval_efficiency(const string &data_dir, const string &tsv_file, int window_size, int stride_counts,
                                   int query_per_update, bool run_fdbcc, const string &damp_dir);


void evaluate_retrieval_efficiency_pct(const string &data_dir, const string &tsv_file, double pct, int stride_count,
                                       bool run_fdbcc, const string &damp_dir);


#endif
