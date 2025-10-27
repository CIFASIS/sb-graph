/*****************************************************************************

 This file is part of SBG Partitioner.

 SBG Partitioner is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Partitioner is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Partitioner.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#include <chrono>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <string>

#include <algorithms/cc/cc.hpp>
#include <util/time_profiler.hpp>

#include "build_sb_graph.hpp"
#include "communication_cost.hpp"
#include "kernighan_lin_partitioner.hpp"
#include "partition_graph.hpp"
#include "partition_graph_cc.hpp"
#include "partition_metrics_api.hpp"
#include "sbg_partitioner_log.hpp"

using namespace std;

using namespace sbg_partitioner;


static void read_configuration_file(const string& filename, PartitionerParams& params)
{
    
    // helper function
    auto to_bool = [](string& str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        std::istringstream is(str);
        bool b;
        is >> std::boolalpha >> b;
        return b;
    };

    ifstream ifs(filename);
    string line;
    while (std::getline(ifs, line)) {
        std::istringstream is_line(line);
        std::string key;
        if(std::getline(is_line, key, '=')) {
            std::string value;
            if(std::getline(is_line, value)) {
                if (key == "filename") {
                    params.filename = value;
                } else if (key == "partitions") {
                    params.number_of_partitions = stoi(value);
                } else if (key == "imbalance") {
                    params.epsilon = stof(value);
                } else if (key == "multithreading") {
                    params.enable_multithreading = to_bool(value);
                } else if (key == "initial_partition_strategy") {
                    params.initial_partition_strategy = InitialPartitionStrategy(stoi(value));
                } else if (key == "metrics") {
                    params.compute_metrics = to_bool(value);
                } else if (key == "partitions_directory") {
                    params.directory = value;
                } else {
                    cerr << "Key " << key << " not found, it will be ignored" << endl;
                }
            }
        }
    }
}


static void usage()
{
    cout << "Usage sbg-partitioner" << endl;
    cout << endl;

    cout << "-c, --config-filename   Path to the configuration file, a file with key/values "
            "that represents the partitioner configuration. If a flag and value by configuration "
            "file are passed, the flag will be used."
         << endl;
    cout << "-f, --filename          Path to the input file, a json file that represents "
          "the model we want to partitionate."
        << endl;
    cout << "-p, --partitions                   Number of partitions." << endl;
    cout << "-h, --help                         Display this information and exit." << endl;
    cout << "-v, --version                      Display version information and exit." << endl;
    cout << "-g                                 Output file path." << endl;
    cout << "-e                                 Imbalance epsilon, a value between 0 and 1." << endl;
    cout << "-m, --compute-metrics              If enabled, computes partition quality metrics." << endl;
    cout << "-d, --directory                    Directory with partitions obtianed by other partitioners, "
          "we want to run quality metrics against them."
        << endl;
    cout << "-t, --enable-multithreading        Enable multithreading during optimization." << endl;
    cout << "-i, --initial-partition-strategy   Choose a particular initial partition strategy. If this "
            "flag is disabaled, all strategies will be computed and the best partition will be chosen.\n"
            "\tValue\tSearching algorithm\tStrategy\tOrder\n"
            "\t0\tDepth first search\tDistributive\tpreorder\n"
            "\t1\tDepth first search\tDistributive\tpostorder\n"
            "\t2\tDepth first search\tGreedy\t\tpreorder\n"
            "\t3\tDepth first search\tGreedy\t\tpostorder\n"
        << endl;

    cout << endl;
    cout << "SBG Partitioner home page: https://github.com/CIFASIS/sbg-partitioner " << endl;
}


static void version()
{
    cout << "SBG Partitioner 1.0.0" << endl;
    cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << endl;
    cout << "This is free software: you are free to change and redistribute it." << endl;
    cout << "There is NO WARRANTY, to the extent permitted by law." << endl;
}


void sort_before_print(PartitionMap partitions, const SBG::LIB::WeightedSBGraph& sb_graph, SBG::LIB::SetAF& set_fact)
{
    for (auto& p : partitions) {
        sort_partition_intervals(p);
    }
    cout << "partitions: " << partitions << endl;
}


void read_directory(const std::string& name, std::vector<std::string>& v)
{
    auto path_leaf_string = [](const std::filesystem::directory_entry& entry) { return entry.path().string(); };

    std::filesystem::path p(name);
    std::filesystem::directory_iterator start(p);
    std::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(v), path_leaf_string);
}


tuple<SBG::LIB::WeightedSBGraph, PartitionMap, double, double> run_current_version(
    const PartitionerParams& params,
    const SBG::LIB::UnordPWMapAF& pw_fact)
{
    auto start_build_graph = chrono::high_resolution_clock::now();
    auto sb_graph = build_sb_graph(params.filename->c_str(), pw_fact);
    auto end_build_graph = chrono::high_resolution_clock::now();
    auto time_to_build_graph = chrono::duration<double, std::milli>(end_build_graph - start_build_graph).count();

    cout << "sb_graph: " << sb_graph << endl;

    auto start_partitionate = chrono::high_resolution_clock::now();
    auto partitions = best_initial_partition(sb_graph, *params.number_of_partitions, params.initial_partition_strategy, params.enable_multithreading);
    cout << "chosen partition " << partitions << endl;

    kl_sbg_imbalance_partitioner(sb_graph, partitions, params.epsilon, params.enable_multithreading);
    auto end_partitionate = chrono::high_resolution_clock::now();
    auto time_to_partitionate = chrono::duration<double, std::milli>(end_partitionate - start_partitionate).count();

    return { sb_graph, partitions, time_to_build_graph, time_to_partitionate };
}


int main(int argc, char** argv)
{
    int opt;
    PartitionerParams params;

    // look for configuration file
    while (true)
    {
        int option_index = 0;
        static struct option long_options[] = {{"config-filename", required_argument, 0, 'c'},
                                                {"filename", required_argument, 0, 'f'},    {"partitions", required_argument, 0, 'p'},
                                                {"output-file", required_argument, 0, 'g'}, {"output-graph", required_argument, 0, 'o'},
                                                {"compute-metrics", no_argument, 0, 'm'},   {"directory", required_argument, 0, 'd'},
                                                {"initial-partition-strategy", required_argument, 0, 'i'}, {"enable-multithreading", no_argument, 0, 't'},
                                                {"version", no_argument, 0, 'v'}, {"help", no_argument, 0, 'h'}};

        opt = getopt_long(argc, argv, "c:f:p:e:o:g:d:i:tmvh:", long_options, &option_index);
        if (opt == EOF) break;

        switch (opt) {
            case 'c':
            if (optarg) {
                params.config_filename = string(optarg);
            }
            break;

            default:
            continue;
        }
    }

    // read configuration file if exists, before reading flags
    if (params.config_filename) {
        read_configuration_file(*params.config_filename, params);
    }
    
    // reset it to read values again. We reed them again because flags overwrite values from configuration file
    optind = 0;
    while (true) {
        int option_index = 0;
        static struct option long_options[] = {{"config-filename", required_argument, 0, 'c'},
                                                {"filename", required_argument, 0, 'f'},    {"partitions", required_argument, 0, 'p'},
                                                {"output-file", required_argument, 0, 'g'}, {"output-graph", required_argument, 0, 'o'},
                                                {"compute-metrics", no_argument, 0, 'm'},   {"directory", required_argument, 0, 'd'},
                                                {"initial-partition-strategy", required_argument, 0, 'i'}, {"enable-multithreading", no_argument, 0, 't'},
                                                {"version", no_argument, 0, 'v'}, {"help", no_argument, 0, 'h'}};

        opt = getopt_long(argc, argv, "c:f:p:e:o:g:d:i:tmvh:", long_options, &option_index);
        if (opt == EOF) break;

        switch (opt) {
            case 'c': // already read
            break;
        
            case 'f':
            if (optarg) {
                params.filename = string(optarg);
            }
            break;

            case 'p':
            if (optarg) {
                params.number_of_partitions = atoi(optarg);
            }
            break;

            case 'o':
            if (optarg) {
                params.output_sb_graph = string(optarg);
            }
            break;

            case 'g':
            if (optarg) {
                params.output_file = string(optarg);
            }
            break;

            case 'e':
            if (optarg) {
                params.epsilon = atof(optarg);
            }
            break;

            case 'm':
            params.compute_metrics = true;
            break;

            case 'd':
            if (optarg) {
                params.directory = string(optarg);
            }
            break;

            case 'i':
            if (optarg) {
                params.initial_partition_strategy = InitialPartitionStrategy(atoi(optarg));
            }
            break;

            case 't':
            params.enable_multithreading = true;
            break;

            case 'v':
            version();
            exit(0);

            case 'h':
            usage();
            exit(0);

            case '?':
            usage();
            exit(-1);
            break;

            default:
            abort();
        }
    }

    if (not params.filename or not params.number_of_partitions) {
        usage();
        exit(1);
    }

    if (params.epsilon < 0 or params.epsilon > 1) {
        usage();
        exit(1);
    }

    cout << "filename is " << *params.filename << endl;
    cout << "number of partitions is " << *params.number_of_partitions << endl;

    optional<string> s;
    if (params.output_sb_graph) {
        s = "";
    }

    SBG::LIB::UnordAF set_fact;
    SBG::LIB::MapAF map_fact(set_fact);
    SBG::LIB::UnordPWMapAF pw_fact(map_fact);

    unique_ptr<SBG::LIB::WeightedSBGraph> sb_graph;
    PartitionMap partitions;
    double time_to_build_graph, time_to_partitionate = 0.0;
    if (false) {
        auto [graph, partitions, time_to_build_graph, time_to_partitionate] = run_current_version(params, pw_fact);
        sb_graph = make_unique<SBG::LIB::WeightedSBGraph>(move(graph));
    } else {
        auto start_build_graph = chrono::high_resolution_clock::now();
        sb_graph = make_unique<SBG::LIB::WeightedSBGraph>(build_sb_graph(params.filename->c_str(), pw_fact));
        auto end_build_graph = chrono::high_resolution_clock::now();
        time_to_build_graph = chrono::duration<double, std::milli>(end_build_graph - start_build_graph).count();

        auto cc_pw_map = SBG::LIB::connectedComponents(*sb_graph);

        sb_graph.reset(new SBG::LIB::WeightedSBGraph(
            sb_graph->fact(),
            cc_pw_map.dom(),
            sb_graph->Vmap(),
            sb_graph->map1(),
            sb_graph->map2(),
            sb_graph->Emap(),
            sb_graph->subEmap()
        ));

        cout << "sb_graph: " << *sb_graph << endl;

        auto start_partitionate = chrono::high_resolution_clock::now();
        partitions = best_initial_partition(*sb_graph, *params.number_of_partitions, params.initial_partition_strategy, params.enable_multithreading);
        cout << "chosen partition " << partitions << endl;

        // create and follow index
        using_cc::SetPointers new_nodes = {};
        vector<using_cc::SetPointers> sorted_partitions;
        unsigned index = 0;
        for (const auto &p : partitions) {
            sorted_partitions.emplace_back();
            for_each(p.cbegin(), p.cend(), [&new_nodes, &sorted_partitions, &index](const auto& s) {
                new_nodes.push_back(using_cc::SetPointer(index++, s, s.cardinal()));
                sorted_partitions.back().push_back(new_nodes.back());
            });
        }

        sbg_partitioner::CommunicationCostCC comm_cc(*sb_graph, new_nodes);

        auto partition_1 = sorted_partitions.at(0), partition_2 = sorted_partitions.at(1);
        sbg_partitioner::using_cc::bisection(*sb_graph, comm_cc, sorted_partitions[0], sorted_partitions[1], cc_pw_map, 0, 0);

        sanity_check(*sb_graph, partitions, *params.number_of_partitions);
    }

    if (not sb_graph) {
        cerr <<  "Something happened and the graph was not even created. Returning earlier" << endl;
        return 1;
    }

    if (params.compute_metrics) {
        map<string, metrics::communication_metrics> metrics;
        int edge_cut = metrics::edge_cut(partitions, *sb_graph, set_fact);
        auto [comm_volume, max_comm_volume] = metrics::communication_volume(partitions, *sb_graph, set_fact, map_fact);
        auto max_imb = metrics::maximum_imbalance(partitions, *sb_graph, set_fact);

        metrics::communication_metrics comm_metrics = metrics::communication_metrics{edge_cut, comm_volume, max_comm_volume, max_imb};
        metrics["sbg-partitioner"] = comm_metrics;

        for (const auto& [f, m] : metrics) {
            cout << f << ": " << m << endl;
        }

        if (params.compute_metrics and params.directory) {
            std::vector<std::string> dir_files;
            read_directory(*params.directory, dir_files);

            for (const auto& f : dir_files) {
                auto partition_from_file = metrics::read_partition_from_file(f, *sb_graph, set_fact);
                int edge_cut = metrics::edge_cut(partition_from_file, *sb_graph, set_fact);
                auto [comm_volume, max_comm_volume] = metrics::communication_volume(partition_from_file, *sb_graph, set_fact, map_fact);
                auto max_imb = metrics::maximum_imbalance(partition_from_file, *sb_graph, set_fact);

                metrics::communication_metrics comm_metrics = metrics::communication_metrics{edge_cut, comm_volume, max_comm_volume, max_imb};
                metrics[std::filesystem::path(f).filename().string()] = comm_metrics;
            }
        }
    }

    // print profiler results if they are enabled
    SBG::Util::time_profiler_results();

    cout << "time_to_build_graph = " << time_to_build_graph << " ms" << endl;
    cout << "time_to_partitionate = " << time_to_partitionate << " ms" << endl;

    if (sanity_check_enabled) {
        sanity_check(*sb_graph, partitions, *params.number_of_partitions);
    }

    if (s) {
        s = get_pretty_sb_graph(*sb_graph);
    }

    sort_before_print(partitions, *sb_graph, set_fact);

    string output = get_output(partitions);

    return 0;
}
