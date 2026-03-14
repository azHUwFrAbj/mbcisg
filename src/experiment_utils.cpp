

#include "../experiment_utils.h"

#include <cassert>
#include <fstream>
#include <random>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>
#include <atomic>

#include "BBFQ.h"
#include "BcTreeQ.h"
#include "holm_lichtenberg_fdbcc/SimHolmBcc.h"


void init(ExperimentRecords &experiment_records, const string &data_set_path, int window_size, int stride_size) {
    experiment_records.settings.data_set_path = data_set_path;
    experiment_records.settings.window_size = window_size;
    experiment_records.settings.stride_size = stride_size;
    experiment_records.settings.stride_count = 0;
    experiment_records.settings.exp_start_time = std::chrono::system_clock::now();
}

void conditional_report(const Timer &timer, const string &title) {
    if (!timer.times().empty()) {
        std::cout << "-- " << title << " --\n";
        timer.status();
    }
}

string f_timestamp(const std::chrono::system_clock::time_point &tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void damp(const Timer &timer, const string &title, const string &damp_file, const ExperimentRecords &settings) {
    if (!timer.times().empty()) {
        std::ofstream outfile(damp_file, std::ios::app);
        vector<double> status = timer.get_status();
        if (outfile.is_open()) {
            outfile << std::fixed << std::setprecision(4);
            outfile << title << "\t" << timer.get_average();
            outfile << "\t" << settings.settings.window_size;
            outfile << "\t" << settings.settings.stride_size;
            outfile << "\t" << settings.settings.stride_count;
            outfile << "\t" << f_timestamp(settings.settings.exp_start_time);

            outfile << "\t" << status.at(1);
            outfile << "\t" << status.at(2);
            outfile << "\t" << status.at(3);
            outfile << "\t" << double(settings.hit_count) / double(settings.total_count);
            outfile << "\n";
        }
    }
}


void report(const ExperimentRecords &experiment_records) {
    using namespace std::chrono;

    std::cout << "========== Experiment Report ==========\n";
    std::cout << "Dataset: " << experiment_records.settings.data_set_path << "\n";
    std::cout << "Window size: " << experiment_records.settings.window_size << "\n";
    std::cout << "Stride size: " << experiment_records.settings.stride_size << "\n";
    std::cout << "Stride count: " << experiment_records.settings.stride_count << "\n";
    std::cout << "Start at: " << f_timestamp(experiment_records.settings.exp_start_time) << "\n";
    std::cout << "Cycle Rate: " << double(experiment_records.hit_count) / double(experiment_records.total_count) <<
            "\n";

    auto duration = duration_cast<seconds>(
        experiment_records.settings.exp_end_time - experiment_records.settings.exp_start_time
    ).count();
    std::cout << "Total experiment time: " << duration << " s\n\n";

    conditional_report(experiment_records.recomputation, "Recomputation");

    conditional_report(experiment_records.block_forest, "Block Forest");

    conditional_report(experiment_records.holm_fdbcc, "Holm FDBCC");

    conditional_report(experiment_records.bbf_basic, "BBF Basic");

    conditional_report(experiment_records.bbf_advance, "BBF Advance");

    conditional_report(experiment_records.bbf_batch, "BBF Batch");

    conditional_report(experiment_records.bbf_advance_with_cache, "BBF Adv Cache");


    std::cout << "=======================================\n";
}

void step(ExperimentRecords &experiment_records) {
    experiment_records.settings.stride_count++;
}

void read_tsv_file(const string &tsv_file, vector<Edge> &edges) {
    edges.clear();

    std::ifstream infile(tsv_file);
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        int source, target;

        if (iss >> source >> target) {
            edges.emplace_back(make_edge(source, target));
        }
    }
}

int get_edge_number(const string &tsv_file) {
    std::ifstream infile(tsv_file);
    std::string line;
    int num = 0;

    while (std::getline(infile, line)) {
        num++;
    }
    return num;
}

void read_tsv_file(const string &tsv_file, queue<Edge> &edges) {
    assert(edges.empty());

    std::ifstream infile(tsv_file);
    std::string line;

    std::getline(infile, line);

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        int source, target;

        if (iss >> source >> target) {
            edges.push(make_edge(source, target));
        }
    }
}

void evaluate_sw_efficiency_pct(const string &data_dir, const string &tsv_file, double window_percentage,
                                int stride_counts, bool run_fdbcc, const string &damp_dir) {
    vector<Edge> edges;
    const string url = data_dir + "/" + tsv_file + ".tsv";
    read_tsv_file(url, edges);

    int window_size = edges.size() * window_percentage;
    int stride_size = 1;

    cout << "Window size: " << window_size << "\n";
    cout << "Stride size: " << stride_size << "\n";


    evaluate_sw_efficiency(data_dir, tsv_file, window_size, stride_size, stride_counts, run_fdbcc, damp_dir);
}

void end(ExperimentRecords &experiment_records) {
    experiment_records.settings.exp_end_time = std::chrono::system_clock::now();
    report(experiment_records);
}

void damp_record(ExperimentRecords &experiment_records, const string &data_damp) {
    damp(experiment_records.recomputation, "Recomp", data_damp, experiment_records);

    damp(experiment_records.block_forest, "BF", data_damp, experiment_records);

    damp(experiment_records.holm_fdbcc, "FD", data_damp, experiment_records);

    damp(experiment_records.bbf_basic, "Basic", data_damp, experiment_records);

    damp(experiment_records.bbf_advance, "Advance", data_damp, experiment_records);

    damp(experiment_records.bbf_advance_with_cache, "Advance_Cache", data_damp, experiment_records);


    damp(experiment_records.bbf_batch, "Batch", data_damp, experiment_records);

    damp(experiment_records.fd_insertion, "HDT_Insertion", data_damp, experiment_records);

    damp(experiment_records.fd_deletion, "HDT_Deletion", data_damp, experiment_records);


    damp(experiment_records.bbf_basic_insertion, "Basic_Insertion", data_damp, experiment_records);

    damp(experiment_records.bbf_basic_deletion, "Basic_Deletion", data_damp, experiment_records);

    damp(experiment_records.bbf_advance_insertion, "Advance_Insertion", data_damp, experiment_records);

    damp(experiment_records.bbf_advance_deletion, "Advance_Deletion", data_damp, experiment_records);

    damp(experiment_records.bbf_advance_insertion_with_cache, "Advance_Insertion_Cache", data_damp, experiment_records);

    damp(experiment_records.bbf_advance_deletion_with_cache, "Advance_Deletion_Cache", data_damp, experiment_records);


    damp(experiment_records.fd_size, "HDT_Size", data_damp, experiment_records);
    damp(experiment_records.bbf_size, "BBF_Size", data_damp, experiment_records);
    damp(experiment_records.bbf_height, "BBF_Height", data_damp, experiment_records);
    damp(experiment_records.bbf_nodes, "BBF_Nodes", data_damp, experiment_records);

    damp(experiment_records.fd_init_size, "HDT_Initial_Size", data_damp, experiment_records);
    damp(experiment_records.bbf_init_size, "BBF_Initial_Size", data_damp, experiment_records);
}


void damp_record_postfix(ExperimentRecords &experiment_records, const string &postifx, const string &data_damp) {
    damp(experiment_records.recomputation, "Recomp_" + postifx, data_damp, experiment_records);

    damp(experiment_records.block_forest, "BF_" + postifx, data_damp, experiment_records);

    damp(experiment_records.holm_fdbcc, "FD_" + postifx, data_damp, experiment_records);

    damp(experiment_records.bbf_basic, "Basic_" + postifx, data_damp, experiment_records);

    damp(experiment_records.bbf_advance_with_cache, "Cache_" + postifx, data_damp, experiment_records);

    damp(experiment_records.bbf_batch, "Batch_" + postifx, data_damp, experiment_records);
}

void damp_query_record(ExperimentRecords &experiment_records, const string &data_damp) {
    damp_record_postfix(experiment_records, "Query", data_damp);
}

void damp_retrieval_record(ExperimentRecords &experiment_records, const string &data_damp) {
    damp_record_postfix(experiment_records, "Retrival", data_damp);
}

void damp_search_record(ExperimentRecords &experiment_records, const string &data_damp) {
    damp_record_postfix(experiment_records, "Search", data_damp);
}

void evaluate_sw_efficiency(const string &data_dir, const string &tsv_file, int window_size, int stride_size,
                            int stride_counts, bool run_fdbcc, const string &damp_dir) {
    std::cout << "\tSliding Window Update Efficiency Experiment Start..." << std::endl;
    std::cout << "\tWindow size: " << window_size << ", stride size" << stride_size << ", stride count:" <<
            stride_counts << std::endl;

    ExperimentRecords records;
    init(records, tsv_file, window_size, stride_size);

    std::cout << "\tLoading " << tsv_file << std::endl;
    vector<Edge> edges;
    const string url = data_dir + "/" + tsv_file + ".tsv";
    read_tsv_file(url, edges);
    const int edge_number = edges.size();
    cout << "Reading " << edge_number << " edges." << endl;

    if (edge_number < window_size) {
        std::cout << "\tHaving edges less than window size " << tsv_file << std::endl;
        return;
    }

    assert(edges.size() >= window_size + stride_size);

    int max_vid = get_max_vid(edges);

#ifdef RECOMPUTE
    BcTreeQ recomp(max_vid);
#endif

    SimHolmBcc sim_holm_bcc(max_vid);

    BBFQ bbf_bsc(max_vid, window_size, edge_number);
    BBFQ bbf_adv(max_vid, window_size, edge_number);
    BBFQ bbf_adv_cache(max_vid, window_size, edge_number);

    std::cout << "\tLoading the edges of the first window to the indexes..." << std::endl;
    vector<Edge> first_edges;


    for (int i = 0; i < window_size; i++) {
        auto eg = edges.at(i);

#ifdef RECOMPUTE
        recomp.add_edge_without_maintaining_bc_tree(eg.first, eg.second);
#endif

        if (run_fdbcc) {
            sim_holm_bcc.insert_edge(eg.first, eg.second);
        }
        first_edges.push_back(eg);
    }

#ifdef RECOMPUTE
    recomp.build_bc_tree();
#endif

    bbf_bsc.insert_batch(first_edges, 0);
    bbf_adv.insert_batch(first_edges, 0);
    bbf_adv_cache.insert_batch(first_edges, 0);

    if (run_fdbcc) {
        cout << "HDT Index Size: " << sim_holm_bcc.get_index_size_in_KB() << " KB" << endl;
    }
    cout << "BBF Index Size: " << bbf_adv.get_index_size_in_KB() << " KB" << endl;
    cout << "BBF Node count: " << bbf_adv.get_bbf_node_count() << endl;
    cout << "BBF Height: " << bbf_adv.get_bbf_height() << endl;
    cout << "Vertex count: " << count_vertices(first_edges) << endl;

    if (run_fdbcc) {
        records.fd_init_size.add(sim_holm_bcc.get_index_size_in_KB());
    }
    records.bbf_init_size.add(bbf_adv.get_index_size_in_KB());


    std::cout << std::fixed << std::setprecision(4);
    int current_stride_number = 0;

    int recomputed = 0;
    const int STRIDE_SIZE_FOR_RECOMP = std::max(1, stride_counts / 100);

    int last_i_for_recomp = window_size - 1;

    cout << "Starting the update efficiency test..." << endl;
    for (int i = window_size; i <= edges.size() - stride_size; i += stride_size) {
        if (current_stride_number >= stride_counts) {
            break;
        }

        cout << "Processing the " << current_stride_number + 1 << "th edge." << endl;

        current_stride_number++;

        vector<Edge> expired_edges;
        expired_edges.reserve(stride_size);
        vector<Edge> batch_edges;
        batch_edges.reserve(stride_size);
        for (int j = 0; j < stride_size; j++) {
            expired_edges.push_back(edges.at(i - window_size + j));
            batch_edges.push_back(edges.at(i + j));
        }

        step(records);
        double progress = ((double) i) / edges.size() * 100.0;
        std::cout << "\t==> Now we move to the " << i << "th edge (" << progress << "%)" << std::endl;

        if (recomputed < 100 && i > last_i_for_recomp + STRIDE_SIZE_FOR_RECOMP) {
            last_i_for_recomp += STRIDE_SIZE_FOR_RECOMP;
            recomputed++;
            cout << "\t--> Recomputation..." << i << "\n";
            vector<vector<VId> > bbcs;
            for (int j = 0; j < stride_size; j++) {
                auto eg = edges.at(i + j);
                recomp.expire_edge_without_maintaining_bc_tree();
                recomp.add_edge_without_maintaining_bc_tree(eg.first, eg.second);
            }

            records.recomputation.start();
            recomp.run_ht_algorithm(bbcs);
            records.recomputation.collect();
        }


        cout << "\t--> BBF Basic...\n";
        int cycle_count = 0;
        records.bbf_basic.start();
        for (int j = 0; j < stride_size; j++) {
            auto eg = edges.at(i + j);

            records.bbf_basic_deletion.start();
            bbf_bsc.expire(i + j - window_size);
            records.bbf_basic_deletion.collect();

            records.bbf_basic_insertion.start();
            bool hit = bbf_bsc.insert_basic(eg, i + j);
            records.bbf_basic_insertion.collect();

            cycle_count += hit;
        }
        records.bbf_basic.collect();
        records.hit_count += cycle_count;

        records.total_count += stride_size;


        cout << "\t--> BBF Advance with Cache...\n";
        records.bbf_advance_with_cache.start();
        for (int j = 0; j < stride_size; j++) {
            auto eg = edges.at(i + j);

            std::vector<Edge> expired_tree_edges;

            records.bbf_advance_deletion_with_cache.start();
            bbf_adv_cache.expire_bbf(i + j - window_size, expired_tree_edges);
            records.bbf_advance_deletion_with_cache.collect();

            bbf_adv_cache.expire_mxt(expired_tree_edges);

            records.bbf_advance_insertion_with_cache.start();
            bbf_adv_cache.insert_adv_with_cache(eg, i + j);
            records.bbf_advance_insertion_with_cache.collect();
        }
        records.bbf_advance_with_cache.collect();


        cout << "\t--> BBF Advance...\n";
        records.bbf_advance.start();
        for (int j = 0; j < stride_size; j++) {
            auto eg = edges.at(i + j);

            std::vector<Edge> expired_tree_edges;

            records.bbf_advance_deletion.start();
            bbf_adv.expire_bbf(i + j - window_size, expired_tree_edges);
            records.bbf_advance_deletion.collect();

            bbf_adv.expire_mxt(expired_tree_edges);

            records.bbf_advance_insertion.start();
            bbf_adv.insert_advance(eg, i + j);
            records.bbf_advance_insertion.collect();
        }
        records.bbf_advance.collect();


        if (run_fdbcc) {
            cout << "\t--> Holm...\n";
            records.holm_fdbcc.start();
            for (int j = 0; j < stride_size; j++) {
                int timestamp = i + j;
                if (timestamp % 1 == 0) {
                    cout << "\t\t--> handling #" << j << " in stride size of size " << stride_size << "\n";
                }
                auto eg = edges.at(i + j);

                records.fd_deletion.start();
                sim_holm_bcc.delete_edge(eg.first, eg.second);
                records.fd_deletion.collect();

                records.fd_insertion.start();
                sim_holm_bcc.insert_edge(eg.first, eg.second);
                records.fd_insertion.collect();
            }
            double bf_time = sim_holm_bcc.brute_force_search_time_ms;
            records.holm_fdbcc.collect(-bf_time);
            sim_holm_bcc.brute_force_search_time_ms = 0;
        }
    }

    if (run_fdbcc) {
        double size = sim_holm_bcc.get_index_size_in_KB();
        records.fd_size.add(size);
    }
    records.bbf_size.add(bbf_adv.get_index_size_in_KB());
    records.bbf_height.add(bbf_adv.get_bbf_height());
    records.bbf_nodes.add(bbf_adv.get_bbf_node_count());

    end(records);
    if (!damp_dir.empty()) {
        damp_record(records, damp_dir + "/" + tsv_file + ".tsv");
    }

    std::cout << "\tEnd of Sliding Window Update Efficiency Experiment, Congratulation!" << std::endl;
}

void evaluate_window_size(const string &data_dir, const string &tsv_file, int stride_size,
                          const std::vector<int> &window_sizes, int stride_counts, bool run_fdbcc,
                          const string &damp_dir) {
    for (auto window_size: window_sizes) {
        evaluate_sw_efficiency(data_dir, tsv_file, window_size, stride_size, stride_counts, run_fdbcc, damp_dir);
    }
}

void evaluate_window_size_ptc(const string &data_dir, const string &tsv_file,
                              const std::vector<double> &percentages, int stride_counts, bool run_fdbcc,
                              const string &damp_dir) {
    for (auto pct: percentages) {
        evaluate_sw_efficiency_pct(data_dir, tsv_file, pct, stride_counts, run_fdbcc, damp_dir);
    }
}

void evaluate_stride_size_pct_fix_ws(const string &data_dir, const string &tsv_file, double window_size_pct,
                                     const std::vector<double> &percentage,
                                     bool run_fdbcc, const string &damp_dir) {
    const string url = data_dir + "/" + tsv_file + ".tsv";
    const int edge_count = get_edge_number(url);
    const int window_size = window_size_pct * edge_count;
    vector<int> stride_size;
    for (auto pct: percentage) {
        stride_size.push_back(window_size * pct);
    }
}


void generate_query_pairs(int query_times, const std::deque<Edge> &current_edges,
                          std::set<std::pair<VId, VId> > &query_pairs) {
    std::set<VId> current_vids;

    for (auto e: current_edges) {
        current_vids.insert(e.first);
        current_vids.insert(e.second);
    }
    assert(current_vids.size() > 2);

    std::vector<VId> query_vids(current_vids.begin(), current_vids.end());
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, query_vids.size() - 1);

    while (query_pairs.size() < query_times) {
        size_t v_index = dist(rng);
        size_t w_index = dist(rng);
        if (v_index != w_index) {
            query_pairs.emplace(query_vids.at(v_index), query_vids.at(w_index));
        }
    }
}

void evaluate_query_efficiency_pct(const string &data_dir, const string &tsv_file, const vector<double> &percentages,
                                   int query_times, int stride_counts, bool run_fdbcc, const string &damp_dir,
                                   double hit_rate) {
    for (auto pct: percentages) {
        const string url = data_dir + "/" + tsv_file + ".tsv";

        int edge_num = get_edge_number(url);
        int window_size = int(edge_num * pct);
        std::cout << "window size: " << window_size << std::endl;
        evaluate_query_efficiency(data_dir, tsv_file, window_size, 1, query_times, stride_counts, run_fdbcc, damp_dir,
                                  hit_rate);
    }
}

void generate_query_pairs(int query_times, const deque<Edge> &current_edges, set<pair<VId, VId> > &query_pairs,
                          double hit_rate, BBFQ &bbfq) {
    std::set<VId> current_vids;

    for (auto e: current_edges) {
        current_vids.insert(e.first);
        current_vids.insert(e.second);
    }
    assert(current_vids.size() > 2);

    std::vector<VId> query_vids(current_vids.begin(), current_vids.end());
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, query_vids.size() - 1);

    const int bar = current_edges.size() * hit_rate;

    while (query_pairs.size() < bar) {
        size_t v_index = dist(rng);
        size_t w_index = dist(rng);
        if (v_index != w_index && bbfq.is_biconnected(v_index, w_index)) {
            query_pairs.emplace(query_vids.at(v_index), query_vids.at(w_index));
        }
    }

    while (query_pairs.size() < query_times) {
        size_t v_index = dist(rng);
        size_t w_index = dist(rng);
        if (v_index != w_index && !bbfq.is_biconnected(v_index, w_index)) {
            query_pairs.emplace(query_vids.at(v_index), query_vids.at(w_index));
        }
    }
}

void evaluate_query_efficiency(const string &data_dir, const string &tsv_file, int window_size, int stride_size,
                               int query_times, int stride_counts, bool run_fdbcc, const string &damp_dir,
                               double hit_rate) {
    std::cout << "\tQuery Efficiency Experiment Start..." << std::endl;

    std::cout << "\tLoading " << tsv_file << std::endl;
    vector<Edge> edges;
    const string url = data_dir + "/" + tsv_file + ".tsv";
    read_tsv_file(url, edges);

    ExperimentRecords records;
    init(records, tsv_file + "_query", window_size, stride_size);

    std::cout << "window_size:\t" << window_size << std::endl;

    assert(edges.size() >= window_size + stride_size);

    int max_vid = get_max_vid(edges);
    SimHolmBcc sim_holm_bcc(max_vid);
    BBFQ bbfq(max_vid, window_size + stride_size + 1);


    deque<Edge> current_edges;
    vector<Edge> first_edges;
    std::cout << "\tLoading the edges of the first window to the indexes..." << std::endl;
    for (int i = 0; i < window_size; i++) {
        current_edges.push_back(edges.at(i));
        first_edges.push_back(edges.at(i));
        auto eg = edges.at(i);
        if (run_fdbcc) {
            sim_holm_bcc.insert_edge(eg.first, eg.second);
        }
    }
    bbfq.insert_batch(first_edges, 0);


    std::cout << std::fixed << std::setprecision(4);

    int current_stride_number = 0;
    for (int i = window_size; i <= edges.size() - stride_size; i += stride_size) {
        if (current_stride_number >= stride_counts) {
            break;
        }
        current_stride_number++;

        step(records);

        double progress = ((double) i) / edges.size() * 100.0;
        std::cout << "\t==> Now we move to the " << i << "th edge (" << progress << "%)" << std::endl;


        for (int j = 0; j < stride_size; j++) {
            current_edges.pop_front();
            current_edges.push_back(edges.at(i + j));
        }

        cout << "\t generating queries..." << std::endl;
        set<pair<VId, VId> > query_pairs;
        if (hit_rate >= 0.0 && hit_rate <= 1.0) {
            generate_query_pairs(query_times, current_edges, query_pairs, hit_rate, bbfq);
        } else {
            generate_query_pairs(query_times, current_edges, query_pairs);
        }


        for (int j = 0; j < stride_size; j++) {
            auto eg = edges.at(i + j);
            }


        vector<Edge> new_edges;
        for (int j = 0; j < stride_size; j++) {
            auto eg_to_add = edges.at(i + j);
            auto eg_to_delete = edges.at(i + j - window_size);
            new_edges.push_back(eg_to_add);
            if (run_fdbcc) {
                sim_holm_bcc.delete_edge(eg_to_add.first, eg_to_add.second);
                sim_holm_bcc.insert_edge(eg_to_add.first, eg_to_add.second);
            }
        }
        if (run_fdbcc) {
            cout << "\t--> Holm...\n";
            for (const auto query_pair: query_pairs) {
                records.holm_fdbcc.start();
                sim_holm_bcc.biconnected(query_pair.first, query_pair.second);
                records.holm_fdbcc.collect();
            }
        }


        cout << "\t--> BBF...\n";
        bbfq.expire(i - window_size + stride_size);
        bbfq.insert_batch(new_edges, i);

        cout << "\t--> BBF without optimization...\n";
        for (const auto query_pair: query_pairs) {
            records.bbf_basic.start();

            bbfq.is_biconnected(query_pair.first, query_pair.second);
            records.bbf_basic.collect();
        }

        }

    end(records);

    if (!damp_dir.empty()) {
        damp_query_record(records, damp_dir + "/" + tsv_file + ".tsv");
    }

    std::cout << "\tEnd of Query Efficiency Experiment, Congratulation!" << std::endl;
}

void evaluate_retrieval_efficiency(const string &data_dir, const string &tsv_file, int window_size,
                                   int stride_counts, int query_per_update, bool run_fdbcc, const string &damp_dir) {
    const int REPEATE_TIME = query_per_update;

    std::cout << "\tRetrieval Efficiency Experiment Start..." << std::endl;

    constexpr int stride_size = 1;
    ExperimentRecords records;
    init(records, tsv_file, window_size, stride_size);

    std::cout << "\tLoading " << tsv_file << std::endl;
    vector<Edge> edges;
    const string url = data_dir + "/" + tsv_file + ".tsv";
    read_tsv_file(url, edges);
    const int edge_number = edges.size();

    assert(edges.size() >= window_size + stride_size);

    int max_vid = get_max_vid(edges);
    BcTreeQ bc_tree(max_vid);
    SimHolmBcc sim_holm_bcc(max_vid);
    BBFQ bbfq(max_vid, window_size + stride_size + 1, edge_number);


    deque<Edge> current_edges;
    vector<Edge> first_edges;
    std::cout << "\tLoading the edges of the first window to the indexes..." << std::endl;
    for (int i = 0; i < window_size; i++) {
        current_edges.push_back(edges.at(i));
        first_edges.push_back(edges.at(i));
        auto eg = edges.at(i);
        bc_tree.add_edge_without_maintaining_bc_tree(eg.first, eg.second);
        if (run_fdbcc) {
            sim_holm_bcc.insert_edge(eg.first, eg.second);
        }
    }
    bbfq.insert_batch(first_edges, 0);
    bc_tree.build_bc_tree();

    cout << "\tloaded the edges of the second window to the indexes..." << std::endl;

    std::cout << std::fixed << std::setprecision(4);
    int current_stride_number = 0;
    for (int i = window_size; i <= edges.size() - stride_size; i += stride_size) {
        if (current_stride_number >= stride_counts) {
            break;
        }
        current_stride_number += 1;

        step(records);

        double progress = ((double) i) / edges.size() * 100.0;
        std::cout << "\t==> Now we move to the " << i << "th edge (" << progress << "%)" << std::endl;


        for (int j = 0; j < stride_size; j++) {
            current_edges.pop_front();
            current_edges.push_back(edges.at(i + j));
        }

        vector<vector<VId> > bbcs;
        for (int j = 0; j < stride_size; j++) {
            auto eg = edges.at(i + j);
            bc_tree.expire_edge_without_maintaining_bc_tree();
            bc_tree.add_edge_without_maintaining_bc_tree(eg.first, eg.second);
        }
        records.recomputation.start();
        bc_tree.run_ht_algorithm(bbcs);
        records.recomputation.collect();


        vector<vector<VId> > vbccs;

        vector<Edge> new_edges;
        for (int j = 0; j < stride_size; j++) {
            auto eg_to_add = edges.at(i + j);
            new_edges.push_back(eg_to_add);
            auto ed_to_delete = edges.at(i + j - window_size);
            if (run_fdbcc) {
                sim_holm_bcc.delete_edge(eg_to_add.first, eg_to_add.second);
                sim_holm_bcc.insert_edge(eg_to_add.first, eg_to_add.second);
            }
        }

        if (run_fdbcc) {
            for (int k = 0; k < REPEATE_TIME; k++) {
                vbccs.clear();
                SimpleTree skeleton = sim_holm_bcc.get_skeleton();
                records.holm_fdbcc.start();
                sim_holm_bcc.get_bccs_dfs(vbccs, skeleton);
                records.holm_fdbcc.collect();
            }
        }

        bbfq.expire(i - window_size + stride_size);
        for (int j = 0; i < new_edges.size(); j++) {
            bbfq.insert_adv_with_cache(new_edges[j], i + j + 1);
        }


        vbccs.clear();
        int root_counts = bbfq.get_root_count();
        vbccs.resize(root_counts);
        bbfq.fresh_roots_vec();


        for (int k = 0; k < REPEATE_TIME; k++) {
            for (auto &block: vbccs) {
                block.clear();
            }

            records.bbf_basic.start();
            bbfq.get_bccs(vbccs);
            records.bbf_basic.collect();
        }

        vbccs.clear();
        vbccs.resize(root_counts);

        for (int k = 0; k < REPEATE_TIME; k++) {
            for (auto &block: vbccs) {
                block.clear();
            }

            records.bbf_advance_with_cache.start();
            bbfq.get_bccs_cache_trick(vbccs);
            records.bbf_advance_with_cache.collect();
        }
    }

    end(records);
    damp_retrieval_record(records, damp_dir + "/" + tsv_file + ".tsv");
    std::cout << "\tEnd of Retrieval Efficiency Experiment, Congratulation!" << std::endl;
}


void evaluate_retrieval_efficiency_pct(const string &data_dir, const string &tsv_file, double pct, int stride_count,
                                       bool run_fdbcc, const string &damp_dir) {
    const string url = data_dir + "/" + tsv_file + ".tsv";
    const int edge_count = get_edge_number(url);
    const int window_size = pct * edge_count > 0 ? pct * edge_count : 1;

    evaluate_retrieval_efficiency(data_dir, tsv_file, window_size, stride_count, 1, run_fdbcc, damp_dir);
}

void evaluate_search_efficiency_pct(const string &data_dir, const string &tsv_file, double pct, int search_count,
                                    bool run_fdbcc, const string &damp_dir) {
    const string url = data_dir + "/" + tsv_file + ".tsv";
    const int edge_count = get_edge_number(url);
    const int window_size = pct * edge_count > 0 ? pct * edge_count : 1;

    evaluate_search_efficiency(data_dir, tsv_file, window_size, search_count, run_fdbcc, damp_dir);
}


void evaluate_search_efficiency(const string &data_dir, const string &tsv_file, int window_size,
                                int search_count, bool run_fdbcc, const string &damp_dir) {
    std::cout << "\tSearch Efficiency Experiment Start..." << std::endl;

    constexpr int stride_size = 1;
    ExperimentRecords records;
    init(records, tsv_file, window_size, stride_size);

    std::cout << "\tLoading " << tsv_file << std::endl;
    vector<Edge> edges;
    const string url = data_dir + "/" + tsv_file + ".tsv";
    read_tsv_file(url, edges);
    const int edge_number = edges.size();

    assert(edges.size() >= window_size + stride_size);

    int max_vid = get_max_vid(edges);

    SimHolmBcc sim_holm_bcc(max_vid);
    BBFQ bbfq(max_vid, window_size + stride_size + 1, edge_number);


    deque<Edge> current_edges;
    vector<Edge> first_edges;
    set<VId> all_vertices_in_first_window;

    std::cout << "\tLoading the edges of the first window to the indexes..." << std::endl;
    for (int i = 0; i < window_size; i++) {
        current_edges.push_back(edges.at(i));
        first_edges.push_back(edges.at(i));
        auto eg = edges.at(i);
        if (run_fdbcc) {
            sim_holm_bcc.insert_edge(eg.first, eg.second);
        }
        all_vertices_in_first_window.insert(eg.first);
        all_vertices_in_first_window.insert(eg.second);
    }
    bbfq.insert_batch(first_edges, 0);
    int max_vid_local = get_max_vid(first_edges);
    HopcroftTarjan ht(max_vid_local);
    for (auto edge: first_edges) {
        ht.add_edge(edge.first, edge.second);
    }

    vector<VId> ran_query_vertices;
    ran_query_vertices.resize(search_count);
    vector<VId> all_vertices_so_far(all_vertices_in_first_window.begin(), all_vertices_in_first_window.end());
    for (int i = 0; i < search_count; i++) {
        ran_query_vertices[i] = all_vertices_so_far.at(rand() % all_vertices_so_far.size());
    }

    vector<vector<VId> > bccs;

    for (VId query_vid: ran_query_vertices) {
        ht.reset_visited();

        records.recomputation.start();
        ht.get_bcc_of_vertex(bccs, query_vid);
        records.recomputation.collect();

        bccs.clear();

        if (run_fdbcc) {
            SimpleTree tree = sim_holm_bcc.get_skeleton();

            records.holm_fdbcc.start();
            sim_holm_bcc.get_bcc_of_vertex(bccs, query_vid, tree);
            records.holm_fdbcc.collect();

            bccs.clear();
        }

        vector<int> timestamps;
        bbfq.get_tree_edges_timestamps_of(query_vid, timestamps);

        records.bbf_basic.start();
        bbfq.get_bcc_of_tree_edges(bccs, timestamps);
        records.bbf_basic.collect();

        bccs.clear();
    }


    end(records);
    damp_search_record(records, damp_dir + "/" + tsv_file + ".tsv");
    std::cout << "\tEnd of Search Efficiency Experiment, Congratulation!" << std::endl;
}


#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>

size_t getCurrentRSSKB() {
    task_basic_info_data_t info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO,
                  reinterpret_cast<task_info_t>(&info), &count) == KERN_SUCCESS) {
        return info.resident_size / 1024;
    }
    return 0;
}

#elif defined(__linux__)
#include <fstream>
#include <string>
#include <sstream>

size_t getCurrentRSSKB() {
    std::ifstream s("/proc/self/status");
    std::string line;
    while (std::getline(s, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(line);
            std::string label, unit;
            size_t kb;
            if (iss >> label >> kb >> unit)
                return kb;
        }
    }
    return 0;
}

#else
#warning "getCurrentRSSKB() not implemented for this platform"
size_t getCurrentRSSKB() { return 0; }
#endif


size_t measurePeakRSSDuring(std::function<void()> task, int interval_ms = 1) {
    std::atomic<bool> running = true;
    size_t start = getCurrentRSSKB();
    size_t peak = start;


    std::thread monitor([&]() {
        while (running) {
            peak = std::max(peak, getCurrentRSSKB());
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
        }
    });

    task();

    running = false;
    monitor.join();
    return (peak > start) ? (peak - start) : 0;
}

void online_computation(const vector<Edge> &edges, const vector<Edge> &new_edges) {
    int max_vid = get_max_vid(edges);
    max_vid = std::max(max_vid, get_max_vid(new_edges)) + 1;

    BcTreeQ recomp(max_vid);

    for (auto eg: edges) {
        recomp.add_edge_without_maintaining_bc_tree(eg.first, eg.second);
    }
    recomp.build_bc_tree();

    for (auto eg: new_edges) {
        recomp.expire_edge_without_maintaining_bc_tree();
        recomp.add_edge_without_maintaining_bc_tree(eg.first, eg.second);
        recomp.build_bc_tree();
    }
}


void build_hdt(const vector<Edge> &edges, const vector<Edge> &new_edges) {
    int max_vid = get_max_vid(edges);
    max_vid = std::max(max_vid, get_max_vid(new_edges)) + 1;

    SimHolmBcc sim_holm_bcc(max_vid);

    for (auto e: edges) {
        sim_holm_bcc.insert_edge(e.first, e.second);
    }

    for (int i = 0; i < new_edges.size(); i++) {
        sim_holm_bcc.delete_edge(edges.at(i).first, edges.at(i).second);
        sim_holm_bcc.insert_edge(new_edges.at(i).first, new_edges.at(i).second);
    }
}


void build_bbf(const vector<Edge> &edges, const vector<Edge> &new_edges) {
    int max_vid = get_max_vid(edges);
    max_vid = std::max(max_vid, get_max_vid(new_edges)) + 1;

    BBFQ bbfq(max_vid, edges.size());
    bbfq.insert_batch(edges, 0);

    int i = 0;
    int j = edges.size();
    for (auto e: edges) {
        bbfq.expire(i);
        bbfq.insert_advance(e, i + j);
        i++;
    }
}
