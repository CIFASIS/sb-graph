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
#include <set>
#include <string>

#include <algorithms/cc/cc.hpp>
#include <sbg/set_fact.hpp>
#include <util/time_profiler.hpp>

#include "build_sb_graph.hpp"
#include "communication_cost.hpp"
#include "kernighan_lin_partitioner.hpp"
#include "partition_graph.hpp"
#include "partition_graph_cc.hpp"
#include "partition_metrics_api.hpp"
#include "sbg_partitioner_log.hpp"
#include "sbg_partitioner_types.hpp"
#include "weighted_sb_graph.hpp"

#ifdef USE_MEMORY_TRACKER
// memory benchmarking
#include <cstdlib>

struct MemoryTracker {
  size_t total_allocated = 0;
  size_t total_freed = 0;
  size_t current_usage() const { return total_allocated - total_freed; }
};

static MemoryTracker tracker;

// Scalar new
void* operator new(size_t size)
{
  tracker.total_allocated += size;
  return std::malloc(size);
}

// Scalar delete
void operator delete(void* memory, size_t size) noexcept
{
  tracker.total_freed += size;
  std::free(memory);
}

// Array new
void* operator new[](size_t size)
{
  tracker.total_allocated += size;
  return std::malloc(size);
}

// Array delete
void operator delete[](void* memory, size_t size) noexcept
{
  tracker.total_freed += size;
  std::free(memory);
}
#endif

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
    if (std::getline(is_line, key, '=')) {
      std::string value;
      if (std::getline(is_line, value)) {
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

void sort_before_print(PartitionMap partitions, const SBG::LIB::WeightedSBGraph& sb_graph)
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

int get_air_conditioners_controller_size(const string& name)
{
  size_t last_underscore = name.find_last_of('_');
  size_t last_dot = name.find_last_of('.');

  if (last_underscore != std::string::npos && last_dot != std::string::npos) {
      // Extract the string between '_' and '.'
      std::string size_str = name.substr(last_underscore + 1, last_dot - last_underscore - 1);
      int size = std::stoi(size_str);
      return size;
  }

    cerr << "the file does not have the expected format" << std::endl;
    throw 1;
}

tuple<unique_ptr<SBG::LIB::WeightedSBGraph>, PartitionMap, double, double> partitionate_traditional(const PartitionerParams& params)
{
  auto start_build_graph = chrono::high_resolution_clock::now();
  unique_ptr<SBG::LIB::WeightedSBGraph> sb_graph_ptr;
  auto filename = *params.filename;
  double time_to_build_graph = 0.;
  if (filename.find("air_conditioners_cont") != std::string::npos) {
    auto size = get_air_conditioners_controller_size(filename);
    auto sb_graph = create_air_conditioners_with_controller_graph(size);
    auto end_build_graph = chrono::high_resolution_clock::now();
    auto time_to_build_graph = chrono::duration<double, std::milli>(end_build_graph - start_build_graph).count();
    sb_graph_ptr = make_unique<SBG::LIB::WeightedSBGraph>(move(sb_graph));
  } else {
    auto sb_graph = build_sb_graph(filename, false);
    sb_graph_ptr = make_unique<SBG::LIB::WeightedSBGraph>(move(sb_graph));
  }

  cout << "sb_graph: " << *sb_graph_ptr << endl;
  cout << "connections:\n";
  for (auto it1 = sb_graph_ptr->map1().begin(), it2 = sb_graph_ptr->map2().begin(); it1 != sb_graph_ptr->map1().end() and it2 != sb_graph_ptr->map2().end(); ++it1, ++it2) {
    auto n1 = (*it1).image();
    auto n2 = (*it2).image();
    cout << n1 << ", " << n2 << "\n";
  }
  cout << endl;

  auto start_partitionate = chrono::high_resolution_clock::now();
  auto partitions = 
      best_initial_partition(*sb_graph_ptr, *params.number_of_partitions, params.initial_partition_strategy, params.enable_multithreading);
  cout << "chosen partition " << partitions << endl;

  kl_sbg_imbalance_partitioner(*sb_graph_ptr, partitions, params.epsilon, params.enable_multithreading);
  auto end_partitionate = chrono::high_resolution_clock::now();
  auto time_to_partitionate = chrono::duration<double, std::milli>(end_partitionate - start_partitionate).count();

  return {move(sb_graph_ptr), partitions, time_to_build_graph, time_to_partitionate};
}

tuple<unique_ptr<SBG::LIB::WeightedSBGraph>, PartitionMap, double, double> partitionate_using_adjacency_matrix(
    const PartitionerParams& params)
{
  auto start_build_graph = chrono::high_resolution_clock::now();
  auto sb_graph = make_unique<SBG::LIB::WeightedSBGraph>(build_sb_graph(params.filename->c_str(), false));
  auto end_build_graph = chrono::high_resolution_clock::now();
  auto time_to_build_graph = chrono::duration<double, std::milli>(end_build_graph - start_build_graph).count();

  cout << "sb_graph: " << *sb_graph << endl;

  auto injective_conn = using_cc::split_nodes_into_injective_domains(*sb_graph);

  logging::sbg_log << "injective connections " << injective_conn << endl;
#if SBG_PARTITIONER_LOGGING
  logging::sbg_log << "remaining " << sb_graph->V().difference(injective_conn) << endl;
#endif

  auto start_partitionate = chrono::high_resolution_clock::now();
  using_cc::SetPointers sorted_nodes = {};
  unsigned index = 0;
  for (const auto& s : injective_conn) {
    sorted_nodes.push_back(using_cc::SetPointer(index++, s, 0, s.cardinal()));
  }

  sbg_partitioner::CommunicationCostCC comm_cc(*sb_graph, sorted_nodes);

  std::vector<sbg_partitioner::using_cc::SetPointers> sorted_partitions;
  auto new_graph = SBG::LIB::WeightedSBGraph(injective_conn, sb_graph->Vmap(), sb_graph->map1(), sb_graph->map2(), sb_graph->Emap(),
                                             sb_graph->subEmap());

  auto non_sorted_partitions =
      best_initial_partition(new_graph, *params.number_of_partitions, params.initial_partition_strategy, params.enable_multithreading);

  // logging::sbg_log << "converting nodes into set pointers" << endl;
  auto start_conversion = chrono::high_resolution_clock::now();
  for (const auto& partition : non_sorted_partitions) {
    sorted_partitions.emplace_back();
    for (const auto& v : partition) {
      for (const auto& n : sorted_nodes) {
        if (not v.intersection(n.set_piece).isEmpty()) {
          size_t offset = v.begin()[0].begin() - n.set_piece.begin()[0].begin();
          sorted_partitions.back().emplace_back(n.index, n.set_piece, offset, v.cardinal());
        }
      }
    }
  }
  auto end_conversion = chrono::high_resolution_clock::now();
  auto conversion_time = chrono::duration<double, std::milli>(end_conversion - start_conversion).count();
  // auto sorted_partitions = using_cc::best_initial_partition(*sb_graph, sorted_nodes, comm_cc, *params.number_of_partitions,
  // InitialPartitionStrategy::DFS_DISTRIBUTIVE_POSTORDER);

  if (sanity_check_enabled) {
    sanity_check(*sb_graph, sbg_partitioner::using_cc::rebuild_partitions(sorted_nodes, sorted_partitions), *params.number_of_partitions);
  }

  using_cc::kl_sbg_imbalance_partitioner(*sb_graph, sorted_nodes, sorted_partitions, comm_cc, 0.);

  auto partitions = sbg_partitioner::using_cc::rebuild_partitions(sorted_nodes, sorted_partitions);
  auto end_partitionate = chrono::high_resolution_clock::now();
  auto time_to_partitionate = chrono::duration<double, std::milli>(end_partitionate - start_partitionate).count();
  time_to_partitionate -= conversion_time;

  if (sanity_check_enabled) {
    sanity_check(*sb_graph, partitions, *params.number_of_partitions);
  }

  return {move(sb_graph), partitions, time_to_build_graph, time_to_partitionate};
}

static struct option long_options[] = {{"config-filename", required_argument, 0, 'c'},
                                       {"filename", required_argument, 0, 'f'},
                                       {"partitions", required_argument, 0, 'p'},
                                       {"output-file", required_argument, 0, 'g'},
                                       {"output-graph", required_argument, 0, 'o'},
                                       {"compute-metrics", no_argument, 0, 'm'},
                                       {"directory", required_argument, 0, 'd'},
                                       {"initial-partition-strategy", required_argument, 0, 'i'},
                                       {"enable-multithreading", no_argument, 0, 't'},
                                       {"use-adjacency-matrix", no_argument, 0, 'k'},
                                       {"version", no_argument, 0, 'v'},
                                       {"help", no_argument, 0, 'h'}};

int main(int argc, char** argv)
{
  int opt;
  PartitionerParams params;

  // look for configuration file
  while (true) {
    int option_index = 0;
    opt = getopt_long(argc, argv, "c:f:p:e:o:g:d:i:ktmvh:", long_options, &option_index);
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
    opt = getopt_long(argc, argv, "c:f:p:e:o:g:d:i:ktmvh:", long_options, &option_index);
    if (opt == EOF) break;

    switch (opt) {
    case 'c':  // already read
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

    case 'k':
      params.use_connected_components = true;
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

  unique_ptr<SBG::LIB::WeightedSBGraph> sb_graph;
  PartitionMap partitions;
  double time_to_build_graph, time_to_partitionate = 0.0;
  if (not params.use_connected_components) {
    tie(sb_graph, partitions, time_to_build_graph, time_to_partitionate) = partitionate_traditional(params);
  } else {
    tie(sb_graph, partitions, time_to_build_graph, time_to_partitionate) = partitionate_using_adjacency_matrix(params);
  }

  if (not sb_graph) {
    cerr << "Something happened and the graph was not even created. Returning earlier" << endl;
    return 1;
  }

  if (params.compute_metrics) {
    map<string, metrics::communication_metrics> metrics;

    int edge_cut = metrics::edge_cut(partitions, *sb_graph);

    auto [comm_volume, max_comm_volume] = metrics::communication_volume(partitions, *sb_graph);

    auto max_imb = metrics::maximum_imbalance(partitions, *sb_graph);

    metrics::communication_metrics comm_metrics = metrics::communication_metrics{edge_cut, comm_volume, max_comm_volume, max_imb};
    metrics["sbg-partitioner"] = comm_metrics;

    if (params.compute_metrics and params.directory) {
      std::vector<std::string> dir_files;
      read_directory(*params.directory, dir_files);

      for (const auto& f : dir_files) {
        if(f.substr(f.find_last_of(".") + 1) == "csv") {
          cout << "Ignoring " << f << endl;
          continue;
        }
        cout << f << endl;

        auto partition_from_file = metrics::read_partition_from_file(f, *sb_graph, *params.number_of_partitions);

        int edge_cut = metrics::edge_cut(partition_from_file, *sb_graph);

        auto [comm_volume, max_comm_volume] = metrics::communication_volume(partition_from_file, *sb_graph);

        auto max_imb = metrics::maximum_imbalance(partition_from_file, *sb_graph);

        metrics::communication_metrics comm_metrics = metrics::communication_metrics{edge_cut, comm_volume, max_comm_volume, };
        metrics[std::filesystem::path(f).filename().string()] = comm_metrics;
      }
    }

    ofstream metrics_file(*params.directory + "/metrics.csv");
    for (const auto& [f, m] : metrics) {
      cout << f << ": " << m << endl;
      metrics_file << f << ", " << m << endl;
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

  sort_before_print(partitions, *sb_graph);

  string output = get_output(partitions);

#ifdef USE_MEMORY_TRACKER
  std::cout << tracker.total_allocated << " bytes were allocated during this execution." << std::endl;
#endif

  return 0;
}
