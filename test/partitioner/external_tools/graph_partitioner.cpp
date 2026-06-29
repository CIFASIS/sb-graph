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

GraphPartitioner::GraphPartitioner(const std::string &name, unsigned int parts) : _name(name), _nbr_parts(parts) { generateInputGraph(); }

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

static void write_node_by_partition(const sbg_partitioner::PartitionMap &partition, const SBG::LIB::WeightedSBGraph &sb_graph,
                                    const std::string &partition_method_name)
{
  std::cout << "writing results of SBG to " << partition_method_name << std::endl;
  sbg_partitioner::Partition nodes;
  nodes.reserve(sb_graph.V().size());
  for (auto v : sb_graph.V()) {
    nodes.push_back(SBG::LIB::SET_FACT.createSet(v));
  }

  sbg_partitioner::sort_partition_intervals(nodes);

  // expand it and write it
  std::ofstream output_file(partition_method_name);
  std::vector<unsigned> partition_vector(sb_graph.V().cardinal(), 0);

  if (sb_graph.V().arity() == 2) {
    size_t row_size = (*sb_graph.V().begin()).intervals()[0].cardinal();
    for (size_t i = 0; i < partition.size(); i++) {
      const auto &p = partition.at(i);
      for (const auto &set : p) {
        for (const auto sp : set) {
          for (size_t row = sp.intervals()[0].begin(); row <= sp.intervals()[0].end(); row++) {
            for (size_t col = sp.intervals()[1].begin(); col <= sp.intervals()[1].end(); col++) {
              auto val = row * row_size + col;
              partition_vector[val] = i;
            }
          }
        }
      }
    }
  } else {
    for (unsigned i = 0; i < partition.size(); i++) {
      for (size_t vert_idx = 0; vert_idx < partition.at(i).size(); vert_idx++) {
        for (size_t val = (*(*partition.at(i).at(vert_idx).begin()).begin()).begin();
             val <= (*(*partition.at(i).at(vert_idx).begin()).begin()).end(); val++) {
          partition_vector[val] = i;
        }
      }
    }
  }

  for_each(partition_vector.begin(), partition_vector.end(), [&output_file](const auto val) { output_file << val << "\n"; });
  std::cout << "parts ok" << std::endl;
}

std::tuple<Partition, std::chrono::duration<double>> GraphPartitioner::createPartition(const std::string &partition_method_name,
                                                                                       unsigned int partitions, bool save_to_file)
{
  std::cout << "GraphPartitioner::createPartition " << partition_method_name << std::endl;
  Partition partition;
  PartitionMethod partition_method = partitionMethod(partition_method_name);

  if (partition_method == PartitionMethod::Unknown) {
    std::cerr << "Unknown partition method, valid values are: " << validPartitionMethodsStr() << std::endl;
    return {partition, std::chrono::duration<double>()};
  }

  partition.resize(_nbr_vtxs);

  _nbr_parts = partitions;

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
      // for now, disabled
      // partitionUsingHMetis(partition);
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

  std::cout << "Partition Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " miliseconds."
            << std::endl;

  if (save_to_file) {
    const std::string fileName =
        _name + "-" + partition_method_name + "-" + std::to_string(_nbr_parts) + "-" + std::to_string(_nbr_vtxs) + ".partition";
    if (partition_method == PartitionMethod::SBG) {
      write_node_by_partition(sbg_partitions, *sbg_graph, fileName);
    } else {
      savePartitionToFile(partition, fileName);
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

namespace {
int get_air_conditioners_controller_size(const std::string &name)
{
  size_t last_underscore = name.find_last_of('_');
  size_t last_dot = name.find_last_of('.');

  if (last_underscore != std::string::npos && last_dot != std::string::npos) {
    // Extract the string between '_' and '.'
    std::string size_str = name.substr(last_underscore + 1, last_dot - last_underscore - 1);
    int size = std::stoi(size_str);
    return size;
  }

  std::cerr << "the file does not have the expected format" << std::endl;
  throw 1;
}
}  // namespace

void GraphPartitioner::readGraphFromJson()
{
  std::cout << "GraphPartitioner::readGraphFromJson" << std::endl;
  if (_name.find("air_conditioners_cont") != std::string::npos) {
    std::cout << "Creating graph for air conditioners with controller" << std::endl;
    auto size = get_air_conditioners_controller_size(_name);
    auto temp_sbg_graph = sbg_partitioner::create_air_conditioners_with_controller_graph(size, _nbr_parts);

    {
      auto new_vertices = sbg_partitioner::split_sets_according_to_relations(temp_sbg_graph);
      sbg_graph.reset(new SBG::LIB::WeightedSBGraph(new_vertices, SBG::LIB::PW_FACT.createPWMap(), temp_sbg_graph.map1().compact(),
                                                    temp_sbg_graph.map2().compact(), SBG::LIB::PW_FACT.createPWMap(),
                                                    SBG::LIB::PW_FACT.createPWMap()));
    }

  } else if (_name.find("advection2D") != std::string::npos) {
    std::cout << "Creating graph for air conditioners with controller" << std::endl;
    auto size = get_air_conditioners_controller_size(_name);
    auto temp_sbg_graph = sbg_partitioner::create_advection2D_graph(size);
    sbg_graph.reset(new SBG::LIB::WeightedSBGraph(temp_sbg_graph));
  } else {
    std::cout << "Creating graph from file " << _name << std::endl;
    auto temp_sbg_graph = sbg_partitioner::build_sb_graph(_name);
    sbg_graph.reset(new SBG::LIB::WeightedSBGraph(temp_sbg_graph));
  }
  std::cout << *sbg_graph << std::endl;

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

void GraphPartitioner::savePartitionToFile(const Partition &partition, const std::string &file_name) const
{
  std::ofstream file(file_name);
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

  auto result = METIS_PartGraphKway(&_nbr_vtxs, &ncon, _xadj.data(), _adjncy.data(), _vwgt.data(), nullptr, _ewgt.data(), &_nbr_parts,
                                    nullptr, nullptr, options.data(), &edgecut, partition.values.data());
  if (result != METIS_OK) {
    std::cerr << "There was an error in METIS" << std::endl;
    throw 1;
  }
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
  SCOTCH_Graph graph_sc;
  SCOTCH_Strat strat;

  SCOTCH_graphInit(&graph_sc);
  SCOTCH_stratInit(&strat);

  if (SCOTCH_stratGraphMapBuild(&strat, SCOTCH_STRATQUALITY, _nbr_parts, 0.05)) {
    std::cerr << "Error building graph partition strategy" << std::endl;
    throw 1;
  }

  if (SCOTCH_graphBuild(&graph_sc, 0, _nbr_vtxs, _xadj.data(), NULL, NULL, NULL, _edges, _adjncy.data(), NULL) != 0) {
    std::cerr << "Error: Scotch Graph Build" << std::endl;
    throw 1;
  }

  if (SCOTCH_graphPart(&graph_sc, _nbr_parts, &strat, partition.values.data()) != 0) {
    std::cerr << "Error: Scotch Graph Partition" << std::endl;
    throw 1;
  }

  SCOTCH_stratExit(&strat);
  SCOTCH_graphExit(&graph_sc);
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
  std::cout << "GraphPartitioner::partitionUsingSBG" << std::endl;
  partitions = sbg_partitioner::best_initial_partition(*sbg_graph, _nbr_parts, sbg_partitioner::InitialPartitionStrategy::ALL, false);
  //   std::cout << "chosen partition " << partitions << std::endl;

  sbg_partitioner::kl_sbg_imbalance_partitioner(*sbg_graph, partitions, _imbalance, false);
}

void GraphPartitioner::addRow(const SBG::LIB::Set &s)
{
  auto edges1 = sbg_graph->map1().preImage(s);
  auto nodes1 = sbg_graph->map2().image(edges1);

  auto edges2 = sbg_graph->map2().preImage(s);
  auto nodes2 = sbg_graph->map1().image(edges2);

  auto nodes = nodes1.cup(nodes2);
  auto nodes_vector = std::vector<int>();
  nodes_vector.reserve(nodes.cardinal());

  size_t row_size = (*(*sbg_graph->V().begin()).begin()).cardinal();

  for (const auto &n : nodes) {
    // std::cout << n << ": ";
    auto it = n.begin();
    auto n1 = *it;
    ++it;
    auto n2 = *it;
    for (auto row = n1.begin(); row <= n1.end(); row++) {
      for (auto col = n2.begin(); col <= n2.end(); col++) {
        // std::cout << row << ", ";
        nodes_vector.push_back(row_size * row + col);
      }
    }
    // std::cout << std::endl;
  }

  sort(nodes_vector.begin(), nodes_vector.end());

  for_each(nodes_vector.cbegin(), nodes_vector.cend(), [this](const auto &val) { _adjncy.push_back(int(val)); });
  _xadj.push_back(_xadj.back() + nodes_vector.size());
}

void GraphPartitioner::readGraphFromSBG()
{
  std::cout << "GraphPartitioner::readGraphFromSBG" << std::endl;
  _nbr_vtxs = sbg_graph->V().cardinal();
  // asumming all setpiece are unidimensional and have step 1

  _xadj.push_back(0);
  std::cout << "arity: " << sbg_graph->V().arity() << std::endl;
  if (sbg_graph->V().arity() == 2) {
    for (const auto &v : sbg_graph->V()) {
      auto it = v.begin();
      const auto &v1 = *it;
      ++it;
      const auto v2 = *it;
#pragma omp parallel for collapse(2)
      for (size_t i = v1.begin(); i <= v1.end(); i++) {
        for (size_t j = v2.begin(); j <= v2.end(); j++) {
          auto s = SBG::LIB::SET_FACT.createSet(SBG::LIB::MultiDimInter({SBG::LIB::Interval(i, 1, i), SBG::LIB::Interval(j, 1, j)}));
          addRow(s);
        }
      }
    }
  } else {
    for (int i = 0; i <= _nbr_vtxs; i++) {
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
  }

  _edges = grp_t(_adjncy.size());

  // @todo: Add logging, for the moment just comment the code.
  /*for (int i = 0; i < _nbr_vtxs; ++i) {
    std::cout << "Node " << i << " Connections: ";
    for (int j = _xadj[i]; j < _xadj[i + 1]; ++j) {
      std::cout << _adjncy[j] << " ";
    }
    std::cout << std::endl;
  }*/

  // @todo: Read weights files.
  _vwgt.resize(_nbr_vtxs, 1);
  _ewgt.resize(_edges, 1);
}