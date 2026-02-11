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

 *****************************************************************************/

#include <array>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unordered_map>
#include <unistd.h>
#include <vector>

#include <kaHIP_interface.h>

#include "graph_partitioner.hpp"
#include <algorithms/partitioner/build_sb_graph.hpp>
#include <algorithms/partitioner/kernighan_lin_partitioner.hpp>
#include <algorithms/partitioner/partition_graph.hpp>
#include <algorithms/partitioner/sbg_partitioner_types.hpp>
#include <algorithms/partitioner/weighted_sb_graph.hpp>

constexpr const char *VALID_PARTITION_METHODS = "{ Scotch, Metis, HMetis, Kahip, SBG }";

// Initialize the map outside the function
static const std::unordered_map<std::string, PartitionMethod> PARTITION_METHOD_MAP = {{"Metis", PartitionMethod::Metis},
                                                                                      {"HMetis", PartitionMethod::HMetis},
                                                                                      {"Scotch", PartitionMethod::Scotch},
                                                                                      {"Kahip", PartitionMethod::Kahip},
                                                                                      {"SBG", PartitionMethod::SBG}};

static std::unique_ptr<SBG::LIB::WeightedSBGraph> sbg_graph;

GraphPartitioner::GraphPartitioner(const std::string &name) : _name(name) { generateInputGraph(); }

std::string GraphPartitioner::validPartitionMethodsStr()
{
  std::ostringstream valid_methods;
  valid_methods << "{";
  for (const auto &pair : PARTITION_METHOD_MAP) {
    if (&pair != &*PARTITION_METHOD_MAP.begin()) {
      valid_methods << ",";
    }
    valid_methods << pair.first;
  }
  valid_methods << "}";
  return valid_methods.str();
}

std::tuple<Partition, std::chrono::duration<double>> GraphPartitioner::createPartition(const std::string &partition_method_name,
                                                                                       unsigned int partitions, bool save_to_file)
{
  Partition partition;
  PartitionMethod partition_method = partitionMethod(partition_method_name);

  if (partition_method == PartitionMethod::Unknown) {
    std::cerr << "Unknown partition method, valid values are: " << validPartitionMethodsStr() << std::endl;
    return {partition, std::chrono::duration<double>()};
  }

  partition.resize(_nbr_vtxs);

  _nbr_parts = partitions;

  partition.resize(_nbr_vtxs);

  // just in case we are using sbg
  sbg_partitioner::PartitionMap sbg_partitions = {};
  sbg_partitions.resize(_nbr_parts);

  auto start = std::chrono::high_resolution_clock::now();

  if (_nbr_vtxs > _nbr_parts) {
    switch (partition_method) {
    case PartitionMethod::Metis:
      partitionUsingMetis(partition);
      break;
    case PartitionMethod::HMetis:
      partitionUsingHMetis(partition);
      break;
    case PartitionMethod::Scotch:
      partitionUsingScotch(partition);
      break;
    case PartitionMethod::Kahip:
      partitionUsingKaHip(partition);
      break;
    case PartitionMethod::SBG:
      partitionUsingSBG(sbg_partitions);
      break;
    default:
      break;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;

  std::cout << "Partition Time: " << duration.count() << " seconds." << std::endl;

  if (save_to_file) {
    if (partition_method == PartitionMethod::SBG) {
      // do something
    } else {
      savePartitionToFile(partition, partition_method_name);
    }
  }

  return {partition, duration};
}

bool GraphPartitioner::endsWithJson() { return _name.size() >= 5 && _name.compare(_name.size() - 5, 5, ".json") == 0; }

void GraphPartitioner::generateInputGraph()
{
  if (endsWithJson()) {
    readGraphFromJson();
  } else {
    readGraph();
  }
}

void GraphPartitioner::readGraphFromJson()
{
  auto temp_sbg_graph = sbg_partitioner::build_sb_graph(_name);
  sbg_graph.reset(new SBG::LIB::WeightedSBGraph(temp_sbg_graph));
  readGraphFromSBG();
}

void GraphPartitioner::readGraph()
{
  const std::string graph_file_name = _name + ".graph";
  const std::string graph_size_file_name = graph_file_name + ".size";

  std::ifstream graph_size_file(graph_size_file_name, std::ios::binary);
  if (!graph_size_file) {
    std::cerr << "Error opening file: " << graph_size_file_name << std::endl;
    return;
  }

  std::ifstream graph_file(graph_file_name, std::ios::binary);
  if (!graph_file) {
    std::cerr << "Error opening file: " << graph_file_name << std::endl;
    return;
  }

  graph_size_file.read(reinterpret_cast<char *>(&_nbr_vtxs), sizeof(_nbr_vtxs));
  graph_size_file.read(reinterpret_cast<char *>(&_edges), sizeof(_edges));

  graph_size_file.close();

  _xadj.resize(_nbr_vtxs + 1);
  _xadj[0] = 0;

  for (int i = 1; i <= _nbr_vtxs; ++i) {
    long read_val;
    graph_file.read(reinterpret_cast<char *>(&read_val), sizeof(long));
    _xadj[i] = read_val;
  }

  _adjncy.resize(_edges, 1);
  for (int i = 0; i < _edges; ++i) {
    long read_val;
    graph_file.read(reinterpret_cast<char *>(&read_val), sizeof(long));
    _adjncy[i] = read_val;
  }

  /* @todo: Add logging, for the moment just comment the code.
  for (int i = 0; i < _nbr_vtxs; ++i) {
    std::cout << "Node " << i << " Connections: ";
    for(int j = _xadj[i]; j < _xadj[i+1]; ++j) {
      std::cout << _adjncy[j] << " ";
    }
    std:: cout << std::endl;
  }*/

  graph_file.close();

  // @todo: Read weights files.
  _vwgt.resize(_nbr_vtxs, 1);
  _ewgt.resize(_edges, 1);
}

PartitionMethod GraphPartitioner::partitionMethod(const std::string &partition_method) const
{
  if (auto it = PARTITION_METHOD_MAP.find(partition_method); it != PARTITION_METHOD_MAP.end()) {
    return it->second;
  }
  return PartitionMethod::Unknown;
}

void GraphPartitioner::savePartitionToFile(const Partition &partition, const std::string &method_name) const
{
  std::string fileName = _name + "-" + method_name + "-" + std::to_string(_nbr_parts) + "-" + std::to_string(_nbr_vtxs) + ".partition";
  std::ofstream file(fileName);
  if (file.is_open()) {
    for (const auto &value : partition.values) {
      file << value << std::endl;
    }
    file.close();
  }
}

void GraphPartitioner::partitionUsingMetis(Partition &partition)
{
  idx_t ncon = 1;
  idx_t edgecut;
  std::array<idx_t, METIS_NOPTIONS> options;

  METIS_SetDefaultOptions(options.data());
  options[METIS_OPTION_CONTIG] = 0;
  options[METIS_OPTION_PTYPE] = METIS_PTYPE_KWAY;
  options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_VOL;
  options[METIS_OPTION_SEED] = 1;

  METIS_PartGraphKway(&_nbr_vtxs, &ncon, _xadj.data(), _adjncy.data(), _vwgt.data(), nullptr, _ewgt.data(), &_nbr_parts, nullptr, nullptr,
                      options.data(), &edgecut, partition.values.data());
}

void GraphPartitioner::partitionUsingHMetis(Partition &partition)
{
  std::string h_graph_name = _name + ".hmetis";
  if (fork() == 0) {
    // Child process
    createHMetisGraphFile(h_graph_name);
    executeKhmetis(h_graph_name);
    std::abort();
  } else {
    wait(nullptr);
    readPartitionFile(h_graph_name, partition);
  }
}

void GraphPartitioner::createHMetisGraphFile(const std::string &file_name)
{
  std::ofstream h_graph_file(file_name);
  if (h_graph_file.is_open()) {
    h_graph_file << _edges << " " << _nbr_vtxs << "\n";
    for (grp_t i = 0; i < _edges; ++i) {
      h_graph_file << _ewgt[i] << " ";
      for (grp_t j = _xadj[i]; j < _xadj[i + 1]; ++j) {
        h_graph_file << _adjncy[j] + 1 << " ";
      }
      h_graph_file << "\n";
    }
    for (grp_t i = 0; i < _nbr_vtxs; ++i) {
      if (!_vwgt.empty()) {
        h_graph_file << _vwgt[i] << "\n";
      }
    }
    h_graph_file.close();
  }
}

void GraphPartitioner::executeKhmetis(const std::string &h_graph_name) const
{
  std::string parts = std::to_string(_nbr_parts);
  execlp("./khmetis", "./khmetis", h_graph_name.c_str(), parts.c_str(), "5", "10", "1", "1", "0", "0", nullptr);
}

void GraphPartitioner::readPartitionFile(const std::string &file_name, Partition &partition) const
{
  std::ifstream part_file(file_name);
  if (!part_file.is_open()) {
    std::cerr << "Error opening file: " << file_name << std::endl;
    return;
  }

  grp_t part;
  while (part_file >> part) {  // Read until the end of the file
    partition.values.push_back(part);
  }
  part_file.close();
}

void GraphPartitioner::partitionUsingScotch(Partition &partition)
{
  SCOTCH_Graph *graph_sc = SCOTCH_graphAlloc();
  if (graph_sc == nullptr) {
    std::cerr << "Error allocating graph" << std::endl;
    return;
  }

  SCOTCH_Strat *strat = SCOTCH_stratAlloc();
  if (SCOTCH_stratInit(strat)) {
    std::cerr << "Error allocating graph" << std::endl;
    return;
  }

  if (SCOTCH_stratGraphMapBuild(strat, SCOTCH_STRATDEFAULT, 4, 0.05)) {
    std::cerr << "Error allocating graph" << std::endl;
    return;
  }

  if (SCOTCH_graphBuild(graph_sc, 0, _nbr_vtxs, _xadj.data(), nullptr, _vwgt.data(), nullptr, _edges, _adjncy.data(), _ewgt.data()) != 0) {
    std::cerr << "Error: Scotch Graph Build" << std::endl;
    return;
  }
  if (SCOTCH_graphPart(graph_sc, _nbr_parts, strat, partition.values.data()) != 0) {
    std::cerr << "Error: Scotch Graph Partition" << std::endl;
    return;
  }

  SCOTCH_stratExit(strat);
  SCOTCH_graphFree(graph_sc);
}

void GraphPartitioner::partitionUsingKaHip(Partition &partition)
{
  int edge_cut = 0;
  int seed = 0;
  const bool SUPPRESS_OUTPUT = false;

  kaffpa(&_nbr_vtxs, nullptr, _xadj.data(), nullptr, _adjncy.data(), &_nbr_parts, &_imbalance, SUPPRESS_OUTPUT, seed, STRONG, &edge_cut,
         partition.values.data());
}

void GraphPartitioner::partitionUsingSBG(sbg_partitioner::PartitionMap &partitions)
{
  partitions = sbg_partitioner::best_initial_partition(*sbg_graph, _nbr_parts, sbg_partitioner::InitialPartitionStrategy::ALL, false);
  // std::cout << "chosen partition " << partitions << std::endl;

  sbg_partitioner::kl_sbg_imbalance_partitioner(*sbg_graph, partitions, _imbalance, false);
}

void GraphPartitioner::readGraphFromSBG()
{
  _nbr_vtxs = sbg_graph->V().cardinal();
  _edges = sbg_graph->E().cardinal();
  // asumming all setpiece are unidimensional and have step 1
  int max_node = -1;
  for (const auto &v : sbg_graph->V()) {
    if (int(v.begin()[0].end()) > max_node) {
      max_node = int(v.begin()[0].end());
    }
  }

  _xadj.push_back(0);

  for (int i = 0; i <= max_node; i++) {
    auto s = SBG::LIB::SET_FACT.createSet(SBG::LIB::Interval(i, 1, i));
    auto edges1 = sbg_graph->map1().preImage(s);
    auto nodes1 = sbg_graph->map2().image(edges1);

    auto edges2 = sbg_graph->map2().preImage(s);
    auto nodes2 = sbg_graph->map1().image(edges2);

    auto nodes = nodes1.cup(nodes2);
    auto nodes_vector = std::vector<int>();
    nodes_vector.reserve(nodes.cardinal());

    for (const auto &n : nodes) {
      for (int val = n.begin()[0].begin(); val <= n.begin()[0].end(); val++) {
        nodes_vector.push_back(val);
      }
    }

    sort(nodes_vector.begin(), nodes_vector.end());

    for_each(nodes_vector.cbegin(), nodes_vector.cend(), [this](const auto &val) { _adjncy.push_back(int(val)); });
    _xadj.push_back(_xadj.back() + nodes_vector.size());
  }

  /*// @todo: Add logging, for the moment just comment the code.
  for (int i = 0; i < _nbr_vtxs; ++i) {
    std::cout << "Node " << i << " Connections: ";
    for(int j = _xadj[i]; j < _xadj[i+1]; ++j) {
      std::cout << _adjncy[j] << " ";
    }
    std:: cout << std::endl;
  }*/

  // @todo: Read weights files.
  _vwgt.resize(_nbr_vtxs, 1);
  _ewgt.resize(_edges, 1);
}