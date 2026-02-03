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

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <vector>

#include "graph_partitioner.hpp"

static int number_of_executions = 15;
static int number_of_cold_executions = 5;

void usage()
{
  std::cout << "Usage: grap_partitioner -f <file_name> -n <num_partitions> -m <partition_method> -i <imbalance>" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -f <file_name>        SBG input JSON file name or generated graph input." << std::endl;
  std::cout << "  -n <num_partitions>   Number of partitions" << std::endl;
  std::cout << "  -m <partition_method> Partition method, possible values: " << GraphPartitioner::validPartitionMethodsStr() << std::endl;
  std::cout << "  -i <imbalance>        Partition imbalance" << std::endl;
  std::cout << "  -h                    Display this information and exit" << std::endl;
}

void parseArgs(int argc, char* argv[], std::string& sbg_json_input, int& partitions, std::string& partition_method, int& imbalance)
{
  int opt;

  while ((opt = getopt(argc, argv, "f:n:m:i:h")) != -1) {
    switch (opt) {
    case 'f':
      sbg_json_input = optarg;
      break;
    case 'n':
      partitions = std::stoi(optarg);
      break;
    case 'm':
      partition_method = optarg;
      break;
    case 'i':
      imbalance = std::stoi(optarg);
      break;
    default:
      usage();
      exit(EXIT_SUCCESS);
    }
  }

  if (sbg_json_input.empty() || partitions <= 0 || partition_method.empty() || imbalance < 0) {
    usage();
    exit(EXIT_FAILURE);
  }
}

void addRecord(const std::string& filename, const std::string& method, int partitions,
               const std::vector<std::chrono::duration<double>>& durations)
{
  // Check if the file already exists before we open it
  bool exists = std::filesystem::exists(filename);

  // Open in append mode
  std::ofstream myFile(filename, std::ios::app);

  if (!myFile.is_open()) {
    std::cerr << "Error opening file!" << std::endl;
    return;
  }

  // If the file is new, write the header first
  if (!exists) {
    myFile << "Method,Partitions";
    for (size_t i = 0; i < number_of_executions - number_of_cold_executions; i++) {
      myFile << ", exec " << i;
    }
    myFile << "\n";
  }

  // Add the new row
  myFile << method << "," << partitions;
  for (const auto& d : durations) {
    myFile << "," << d.count();
  }
  myFile << "\n";

  myFile.close();
  std::cout << "Results in " << filename << std::endl;
}

int main(int argc, char* argv[])
{
  std::string json_file_name;
  int partitions;
  std::string partition_method;
  int imbalance;

  parseArgs(argc, argv, json_file_name, partitions, partition_method, imbalance);

  std::vector<std::chrono::duration<double>> durations;

  for (size_t i = 0; i < number_of_executions; i++) {
    GraphPartitioner partitioner(json_file_name);
    auto [partition, duration] = partitioner.createPartition(partition_method, partitions, i == number_of_executions - 1);

    if (i < number_of_cold_executions) {
      continue;
    }

    durations.push_back(duration);
  }

  std::filesystem::path json_file_name_path(json_file_name);

  addRecord(json_file_name_path.filename().replace_extension("csv"), partition_method, partitions, durations);

  // write results to a file
  // std::filesystem::path json_filesystem_path(json_file_name);
  // json_filesystem_path = json_filesystem_path.replace_extension();
  // std::ofstream output_file(json_filesystem_path.filename().string() + "_" + partition_method + "_" + std::to_string(partitions) +
  //                           "_output.txt");
  // std::for_each(partition.values.cbegin(), partition.values.cend(),
  //               [&output_file](const auto& val) { output_file << std::to_string(val) << "\n"; });

  return 0;
}