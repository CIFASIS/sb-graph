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

#include "build_sb_graph.hpp"
#include "kernighan_lin_partitioner.hpp"
#include "partition_graph.hpp"
#include "partition_metrics_api.hpp"


using namespace std;

using namespace sbg_partitioner;


static void usage()
{
  cout << "Usage sbg-partitioner" << endl;
  cout << endl;
  cout << "-f, --filename          Path to the input file, a json file that represents "
          "the model we want to partitionate."
       << endl;
  cout << "-p, --partitions        Number of partitions." << endl;
  cout << "-h, --help              Display this information and exit." << endl;
  cout << "-v, --version           Display version information and exit." << endl;
  cout << "-g                      Output file path." << endl;
  cout << "-e                      Imbalance epsilon, a value between 0 and 1." << endl;
  cout << "-m, --compute-metrics   If enabled, computes partition quality metrics." << endl;
  cout << "-d, --directory         Directory with partitions obtianed by other partitioners, "
          "we want to run quality metrics against them." << endl;
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
    auto path_leaf_string = [](const std::filesystem::directory_entry& entry) {
        return entry.path().string();
    };

    std::filesystem::path p(name);
    std::filesystem::directory_iterator start(p);
    std::filesystem::directory_iterator end;
    std::transform(start, end, std::back_inserter(v), path_leaf_string);
}


int main(int argc, char** argv)
{
    int opt;
    optional<string> filename = nullopt;
    optional<string> directory = nullopt;
    optional<unsigned> number_of_partitions = nullopt;
    optional<string> output_file;
    optional<string> output_sb_graph = nullopt;
    optional<float> epsilon = nullopt;
    bool compute_metrics = false;

    while (true) {
        static struct option long_options[] = {{"filename", required_argument, 0, 'f'},    {"partitions", required_argument, 0, 'p'},
                                               {"output-file", required_argument, 0, 'g'}, {"output-graph", required_argument, 0, 'o'},
                                               {"compute-metrics", no_argument, 0, 'm'}, {"directory", required_argument, 0, 'd'}, 
                                               {"version", no_argument, 0, 'v'}, {"help", no_argument, 0, 'h'}};

        int option_index = 0;
        opt = getopt_long(argc, argv, "f:p:e:o:g:d:mvh:", long_options, &option_index);
        if (opt == EOF) break;

        switch (opt) {
        case 'f':
        if (optarg) {
            filename = string(optarg);
        }
        break;

        case 'p':
        if (optarg) {
            number_of_partitions = atoi(optarg);
        }
        break;

        case 'o':
        if (optarg) {
            output_sb_graph = string(optarg);
        }
        break;

        case 'g':
        if (optarg) {
            output_file = string(optarg);
        }
        break;

        case 'e':
        if (optarg) {
            epsilon = atof(optarg);
        }
        break;

        case 'm':
        compute_metrics = true;
        break;

        case 'd':
        if (optarg) {
            directory = string(optarg);
        }
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
        cout << "opt " << opt << endl;
        abort();
        }
    }

    if (not filename or not number_of_partitions) {
        usage();
        exit(1);
    }

    if (not epsilon) {
        epsilon = 0.0;
    }

    if (*epsilon < 0 or *epsilon > 1) {
        usage();
        exit(1);
    }

    cout << "filename is " << *filename << endl;
    cout << "number of partitions is " << *number_of_partitions << endl;

    optional<string> s;
    if (output_sb_graph) {
        s = "";
    }

    SBG::LIB::UnordAF set_fact;
    SBG::LIB::MapAF map_fact(set_fact);
    SBG::LIB::UnordPWMapAF pw_fact(map_fact);

    auto start_build_graph = chrono::high_resolution_clock::now();
    auto sb_graph = build_sb_graph(filename->c_str(), set_fact, map_fact, pw_fact);
    auto end_build_graph = chrono::high_resolution_clock::now();
    auto time_to_build_graph = chrono::duration<double, std::milli>(end_build_graph - start_build_graph).count();

    auto start_partitionate = chrono::high_resolution_clock::now();
    auto partitions = best_initial_partition(sb_graph, *number_of_partitions, set_fact);
    kl_sbg_imbalance_partitioner(sb_graph, partitions, *epsilon, set_fact, map_fact);
    auto end_partitionate = chrono::high_resolution_clock::now();
    auto time_to_partitionate = chrono::duration<double, std::milli>(end_partitionate - start_partitionate).count();

    if (compute_metrics) {
        map<string, metrics::communication_metrics> metrics;

        int edge_cut = metrics::edge_cut(partitions, sb_graph, set_fact);
        cout << edge_cut << endl;

        auto [comm_volume, max_comm_volume] = metrics::communication_volume(partitions, sb_graph, set_fact, map_fact);

        cout << comm_volume << ", " << max_comm_volume << endl;

        auto max_imb = metrics::maximum_imbalance(partitions, sb_graph, set_fact);

        metrics::communication_metrics comm_metrics = metrics::communication_metrics{ edge_cut, comm_volume, max_comm_volume, max_imb };
        metrics["sbg-partitioner"] = comm_metrics;

        for (const auto& [f, m] : metrics) {
            cout << f << ": " << m << endl;
        }

        if (compute_metrics and directory) {
            std::vector<std::string> dir_files;
            read_directory(*directory, dir_files);

            for (const auto& f : dir_files) {
                auto partition_from_file = metrics::read_partition_from_file(f, sb_graph, set_fact);

                int edge_cut = metrics::edge_cut(partition_from_file, sb_graph, set_fact);

                auto [comm_volume, max_comm_volume] = metrics::communication_volume(partition_from_file, sb_graph, set_fact, map_fact);

                auto max_imb = metrics::maximum_imbalance(partition_from_file, sb_graph, set_fact);

                metrics::communication_metrics comm_metrics = metrics::communication_metrics{ edge_cut, comm_volume, max_comm_volume, max_imb };
                metrics[std::filesystem::path(f).filename().string()] = comm_metrics;
            }
        }
    }

    cout << "time_to_build_graph = " << time_to_build_graph << " ms" << endl;
    cout << "time_to_partitionate = " << time_to_partitionate << " ms" << endl;

    if (sanity_check_enabled) {
        sanity_check(sb_graph, partitions, *number_of_partitions, set_fact);
    }

    if (s){
        s = get_pretty_sb_graph(sb_graph);
    }

    sort_before_print(partitions, sb_graph, set_fact);

    string output = get_output(partitions);

    return 0;
}
